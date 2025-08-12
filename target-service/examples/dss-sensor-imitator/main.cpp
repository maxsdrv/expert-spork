#include "Defs.h"

#include "dss-target-service/Paths.h"

#include "ApiRestServer.h"
#include "DiagnosticDataGenerator.h"
#include "SensorTargetDataGenerator.h"
#include "Settings.h"
#include "WsSensorTargetPublishService.h"

#include "channel/ChannelInfoApiAdapter.h"
#include "diagnostic/DiagnosticApiAdapter.h"
#include "diagnostic/DiagnosticController.h"
#include "jammer/JammerApiAdapter.h"

#include "geolocation/GeolocationApiAdapter.h"
#include "geolocation/GeolocationController.h"

#include "log/Log2.h"

#include <boost/program_options.hpp>

#include <QCoreApplication>

#include <chrono>
#include <memory>
#include <regex>

using RestServerPtr = std::unique_ptr<mpk::dss::ApiRestServer>;
RestServerPtr createRestServer(int restPort)
{
    using namespace mpk::dss;

    auto connectionSettings =
        http::ConnectionSettings{.host = "0.0.0.0", .port = restPort};
    auto server = std::make_unique<ApiRestServer>(
        connectionSettings, appkit::configFile("dss-sensor-imitator-api.json"));
    server->run();

    return server;
}

using WsServerPtr = std::unique_ptr<mpk::dss::WsSensorTargetPublishService>;
WsServerPtr createWsServer(
    int wsPort,
    gsl::not_null<mpk::dss::SensorTargetDataGenerator*> sensorTargetGenerator,
    gsl::not_null<mpk::dss::DiagnosticDataGenerator*> diagnosticDataGenerator,
    std::chrono::seconds publishPeriod)
{
    using namespace mpk::dss;

    auto connectionSettings =
        http::ConnectionSettings{.host = "0.0.0.0", .port = wsPort};

    auto server = std::make_unique<WsSensorTargetPublishService>(
        connectionSettings,
        sensorTargetGenerator,
        diagnosticDataGenerator,
        publishPeriod);

    return server;
}

int main(int argc, char* argv[])
{
    QCoreApplication application(argc, argv);

    using namespace mpk::dss;

    auto geolocation = backend::rest::Geolocation{};
    auto settings = Settings{};
    auto channels = std::vector<backend::rest::SingleChannelInfo>{};
    auto jammers = std::vector<core::JammerDevice>{};
    auto diagnostic = backend::rest::DeviceDiagnosticData{};
    bool hasTargetPosition = true;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help", "display help message")(
        "location",
        po::value<decltype(geolocation.location)>(&geolocation.location)
            ->default_value(geolocation.location),
        "set location in format: \'30.23 60.23 10.10\' or 30.23,60.23,10.10")(
        "azimuth",
        po::value<decltype(geolocation.azimuth)>(&geolocation.azimuth)
            ->default_value(geolocation.azimuth),
        "set azimuth")(
        "add_channel",
        po::value<std::vector<std::string>>()->multitoken(),
        R"(add channel with range, min and max angle in format '25.5 -90 90' or '25.5,-90,90')")(
        "add_jammer",
        po::value<std::vector<std::string>>()->multitoken(),
        "add jammer with string id, mode, state and timeout in format \'jammer_1 auto on 1000' or '\'jammer_1, auto, on, 1000")(
        "timeout",
        po::value<decltype(settings.timeout)>(&settings.timeout)
            ->default_value(settings.timeout),
        "set target broadcast timeout in seconds")(
        "http_port",
        po::value<decltype(settings.httpPort)>(&settings.httpPort)
            ->default_value(settings.httpPort),
        "set REST API port")(
        "websocket_port",
        po::value<decltype(settings.websocketPort)>(&settings.websocketPort)
            ->default_value(settings.websocketPort),
        "set Websocket port")(
        "target_postion_enabled",
        po::value<decltype(hasTargetPosition)>(&hasTargetPosition)
            ->default_value(hasTargetPosition),
        "set target position in tracks enabled (true) or disabled (false)");

    po::variables_map vm;

    try
    {
        po::command_line_parser parser(argc, argv);
        parser.options(desc);
        auto parsed_options = parser.run();

        for (const auto& opt: parsed_options.options)
        {
            if (opt.string_key == "add_channel")
            {
                if (opt.value.size() == 1)
                {
                    const auto& str = opt.value.front();
                    auto regex = std::regex{R"([\s,]+)"};
                    auto it = std::sregex_token_iterator{
                        str.begin(), str.end(), regex, -1};

                    if (std::distance(it, std::sregex_token_iterator{}) == 3)
                    {
                        channels.push_back(backend::rest::SingleChannelInfo{
                            .range = std::stof(*it++),
                            .viewAngle = {std::stof(*it++), std::stof(*it)},
                            .jammerIds = backend::rest::JammerIds{}});
                        channels.back().jammerIds.emplace_back(
                            QUuid::createUuid().toString());
                        channels.back().jammerIds.emplace_back(
                            QUuid::createUuid().toString());

                        continue;
                    }
                }

                std::cerr << "Incorrect channels format \n";
                return EXIT_FAILURE;
            }
            if (opt.string_key == "add_jammer")
            {
                // TODO: fix
                continue;
                /*if (opt.value.size() == 1)
                {
                    const auto& str = opt.value.front();
                    auto regex = std::regex{R"([\s,]+)"};
                    auto it = std::regex_token_iterator{
                        str.begin(), str.end(), regex, -1};
                    if (std::distance(it, std::sregex_token_iterator{}) == 3)
                    {
                        core::JammerClient::BandList bandList{
                            {(*it++).str().data()}};
                        jammers.emplace_back(
                            std::make_unique<JammerMockup>(bandList));
                        continue;
                    }
                }*/

                std::cerr << "Incorrect jammers format \n";
                return EXIT_FAILURE;
            }
        }

        po::store(parsed_options, vm);
        po::notify(vm);
    }
    catch (const po::error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help") != 0)
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    try
    {
        auto restServer = createRestServer(settings.httpPort);

        auto geolocationProvider =
            std::make_unique<GeolocationController>(geolocation);
        auto geolocationApiAdapter = std::make_unique<GeolocationApiAdapter>(
            *restServer->apiGateway(), geolocationProvider.get());
        auto channelApiAdapter = std::make_unique<ChannelInfoApiAdapter>(
            *restServer->apiGateway(), channels);
        auto diagnosticProvider =
            std::make_unique<DiagnosticController>(diagnostic);
        auto diagnosticApiAdapter = std::make_unique<DiagnosticApiAdapter>(
            *restServer->apiGateway(), diagnosticProvider.get());
        auto jammerApiAdapter = std::make_unique<JammerApiAdapter>(
            *restServer->apiGateway(), std::move(jammers));

        auto sensorTargetDataGenerator =
            std::make_unique<SensorTargetDataGenerator>(
                geolocationProvider.get(), channels, hasTargetPosition);

        auto diagnosticDataGenerator =
            std::make_unique<DiagnosticDataGenerator>(diagnosticProvider.get());

        auto wsServer = createWsServer(
            settings.websocketPort,
            sensorTargetDataGenerator.get(),
            diagnosticDataGenerator.get(),
            std::chrono::seconds(settings.timeout));

        return application.exec();
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << ex.what();
        return EXIT_FAILURE;
    }
}
