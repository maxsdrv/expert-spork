#include "Defs.h"
#include "dss-target-service/Paths.h"

#include "dss-backend/BackendComponents.h"
#include "dss-backend/BackendController.h"
#include "dss-backend/BackendSignals.h"
#include "dss-backend/CameraSignals.h"
#include "dss-backend/Configuration.h"
#include "dss-backend/exceptions/All.h"
#include "dss-common/config/ConnectionSettings.h"
#include "dss-common/config/ConnectionSettingsEnv.h"
#include "dss-core/CameraApiAdapter.h"
#include "dss-core/CommonApiAdapter.h"
#include "dss-core/DeviceApiAdapter.h"
#include "dss-core/DeviceIdMappingStorage.h"
#include "dss-core/DevicePositionStorage.h"
#include "dss-core/DevicesDisabledSet.h"
#include "dss-core/DevicesRegisteredConnect.h"
#include "dss-core/Domains.h"
#include "dss-core/JammerApiAdapter.h"
#include "dss-core/JammerBand.h"
#include "dss-core/JammerDevice.h"
#include "dss-core/JammerGroupProvider.h"
#include "dss-core/JammerMode.h"
#include "dss-core/JammerNotifier.h"
#include "dss-core/SensorApiAdapter.h"
#include "dss-core/SensorDevice.h"
#include "dss-core/SensorNotifier.h"
#include "dss-core/TargetApiAdapter.h"
#include "dss-core/TargetController.h"
#include "dss-core/TargetNotifier.h"
#include "dss-core/TrackController.h"
#include "dss-core/WsPublisher.h"
#include "dss-core/YamlConvertHelper.h"
#include "dss-core/database/DbServiceFactory.h"
#include "dss-core/database/DeviceDisabledDbHelper.h"
#include "dss-core/license/LicenseController.h"
#include "dss-core/rest/ApiRestServer.h"

#include "exception/ErrorInfo.h"
#include "exception/General.h"
#include "exception/KeyInfo.h"

#include "log/Log2.h"

#include <QCoreApplication>
#include <QObject>
#include <QStringLiteral>

#include <boost/program_options.hpp>

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>

using namespace mpk::dss::backend;
using namespace mpk::dss::core;

namespace
{

void connectWSPublisher(
    gsl::not_null<WsPublisher*> wsPublisher,
    gsl::not_null<SensorNotifier*> sensorNotifier,
    gsl::not_null<JammerNotifier*> jammerNotifier,
    gsl::not_null<TargetNotifier*> targetNotifier,
    gsl::not_null<LicenseController*> licenseController)
{
    QObject::connect(
        sensorNotifier,
        &mpk::dss::core::SensorNotifier::notify,
        wsPublisher,
        [wsPublisher](const auto& message, const auto& data)
        { wsPublisher->sendNotification(Domain::Sensor, message, data); });

    QObject::connect(
        jammerNotifier,
        &mpk::dss::core::JammerNotifier::notify,
        wsPublisher,
        [wsPublisher](const auto& message, const auto& data)
        { wsPublisher->sendNotification(Domain::Jammer, message, data); });

    QObject::connect(
        targetNotifier,
        &mpk::dss::core::TargetNotifier::notify,
        wsPublisher,
        [wsPublisher](const auto& message, const auto& data)
        { wsPublisher->sendNotification(Domain::Target, message, data); });

    const auto* licenseStatusChangedMsg = "license_status";
    QObject::connect(
        licenseController,
        &mpk::dss::core::LicenseController::statusUpdated,
        wsPublisher,
        [wsPublisher, licenseStatusChangedMsg](
            const OAService::OAServiceLicense_status& status)
        {
            if (status.isRValid())
            {
                wsPublisher->sendNotification(
                    Domain::General,
                    licenseStatusChangedMsg,
                    status.asJsonObject());
            }
        });
    QObject::connect(
        licenseController,
        &mpk::dss::core::LicenseController::licenseError,
        wsPublisher,
        [wsPublisher, licenseStatusChangedMsg](
            const OAService::OAServiceLicense_status& status)
        {
            wsPublisher->sendNotification(
                Domain::General,
                licenseStatusChangedMsg,
                status.asJsonObject());
        });
}

void connectTrackController(
    gsl::not_null<BackendController*> backendController,
    gsl::not_null<TrackController*> trackController,
    gsl::not_null<LicenseController*> licenseController)
{
    QObject::connect(
        backendController->pin(),
        &mpk::dss::backend::BackendSignals::trackStarted,
        trackController,
        [validator = licenseController.get(),
         controller = trackController.get()](
            const mpk::dss::core::DeviceId& deviceId,
            const mpk::dss::core::TrackId& trackId,
            const OAService::OAServiceCamera_track& cameraTrack,
            const mpk::dss::core::AlarmStatus& alarmStatus,
            const mpk::dss::core::TargetClass::Value& className,
            const QJsonObject& attributes)
        {
            if (validator->status().isRValid())
            {
                controller->handleTrackStart(
                    deviceId,
                    trackId,
                    cameraTrack,
                    alarmStatus,
                    className,
                    attributes);
            }
        });

    QObject::connect(
        backendController->pin(),
        &mpk::dss::backend::BackendSignals::trackUpdated,
        trackController,
        [validator = licenseController.get(),
         controller = trackController.get()](
            const mpk::dss::core::DeviceId& deviceId,
            const mpk::dss::core::TrackId& trackId,
            const mpk::dss::core::AlarmStatus& alarmStatus,
            const mpk::dss::core::TargetClass::Value& className,
            const QJsonObject& attributes,
            const mpk::dss::core::TrackPoints& trackPoints,
            const qint64& detectCount)
        {
            if (validator->status().isRValid())
            {
                controller->handleTrackUpdate(
                    deviceId,
                    trackId,
                    alarmStatus,
                    className,
                    attributes,
                    trackPoints,
                    detectCount);
            }
        });

    QObject::connect(
        backendController->pin(),
        &mpk::dss::backend::BackendSignals::trackFinished,
        trackController,
        [validator = licenseController.get(),
         controller = trackController.get()](
            const mpk::dss::core::DeviceId& deviceId,
            const mpk::dss::core::TrackId& trackId)
        {
            if (validator->status().isRValid())
            {
                controller->handleTrackFinish(deviceId, trackId);
            }
        });
}

} // namespace

int main(int argc, char** argv)
{
    auto appName = std::string{"DSS Target Service"};
    // clang-format off
    auto descriptionString =
        std::string{"DSS target provider is software package that is designed\n"}
            + "to aggregate sensor data and publish it to consumers via REST API and WebSockets.\n";

    auto copyrightString =
        std::string{"Copyright (C) TSNK, 2023-"} + BUILD_YEAR + ", Moscow, Russia\n"
            + "Copyright (C) MPK Software, 2023-" + BUILD_YEAR + ", Saint-Petersburg, Russia\n";

    auto versionString = std::string{"Version: "} + VERSION + "-" + REVISION;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "display help message")
        ("version", "print version");
#ifdef ALLOW_DISABLE_LICENSE
    desc.add_options()
        ("disable-license", "disable license check");
#endif
    // clang-format on

    po::variables_map vm;

    try
    {
        po::command_line_parser parser(argc, argv);
        parser.options(desc);
        auto parsed_options = parser.allow_unregistered().run();

#ifndef QT_QML_DEBUG
        auto unrecognized = po::collect_unrecognized(
            parsed_options.options, po::include_positional);
        if (!unrecognized.empty())
        {
            std::cout << "Unknown option: " << *unrecognized.begin()
                      << std::endl;
            std::cout << std::endl << desc << std::endl;
            return EXIT_FAILURE;
        }
#endif
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

    if (vm.count("version") != 0)
    {
        std::cout << descriptionString << std::endl;
        std::cout << copyrightString << std::endl;
        std::cout << appName << std::endl;
        std::cout << versionString << std::endl;
        return EXIT_SUCCESS;
    }

    bool disableLicense = false;
#ifdef ALLOW_DISABLE_LICENSE
    if (vm.count("disable-license") != 0)
    {
        disableLicense = true;
    }
#endif

    try
    {
        std::filesystem::create_directories(appkit::tmpPath());
        std::filesystem::create_directories(appkit::dumpsPath());
    }
    catch (const std::exception& exception)
    {
        std::cerr << "An error occurred while creating directories: "
                  << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    YAML::Node config;
    auto configFile = appkit::configFile("dss-target-service.yaml");
    LOG_DEBUG << "Config path: " << configFile;
    try
    {
        config = YAML::LoadFile(configFile);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "A error occurred while reading " << configFile << ": "
                  << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    using namespace appkit;
    std::unique_ptr<logger::Log2> logger;
    auto logIniFile = appkit::configFile("dss-target-service.log.ini");
    try
    {
        logger = std::make_unique<logger::Log2>(logIniFile);
    }
    catch (const std::exception& exception)
    {
        std::cerr << "An error occurred while configuring the log. "
                  << exception.what() << ". Log ini file: " << logIniFile
                  << std::endl;
        return EXIT_FAILURE;
    }

    LOG_INFO << appName;
    LOG_INFO << versionString;

    try
    {
        QCoreApplication::setSetuidAllowed(true);
        QCoreApplication application(argc, argv);
        QCoreApplication::setOrganizationName(QStringLiteral("MPK Software"));
        QCoreApplication::setApplicationName(
            QStringLiteral("dss-target-service"));

        auto* licensePath = std::getenv("LICENSE_FILE");
        if (nullptr == licensePath)
        {
            LOG_FATAL << "Missing LICENSE_FILE environment variable. Exiting";
            return EXIT_FAILURE;
        }
        const auto licenseFileName =
            strings::fromUtf8(std::string(licensePath));
        const auto licenseUpdateInterval = 60000;
        auto licenseController = std::make_unique<LicenseController>(
            licenseFileName, licenseUpdateInterval, disableLicense);

        auto httpConnectionSettings =
            mpk::dss::config::ConnectionSettingsEnv("HTTP_LISTEN");
        auto wsConnectionSettings =
            mpk::dss::config::ConnectionSettingsEnv("WS_LISTEN");

        auto restServer = std::make_unique<ApiRestServer>(
            httpConnectionSettings,
            appkit::configFile("dss-target-service-api.json"));
        restServer->run();

        auto dbService = createDbService();
        auto deviceIdMappingStorage =
            std::make_shared<DeviceIdMappingStorage>(dbService);
        auto devicePositionStorage =
            std::make_shared<DevicePositionStorage>(dbService);

        auto jammerGroupProvider = std::make_shared<JammerGroupProvider>();

        auto backendControllers = createBackend(
            config,
            BackendComponents{
                deviceIdMappingStorage,
                devicePositionStorage,
                jammerGroupProvider.get(),
                licenseController.get()});
        LOG_DEBUG << "Controllers loaded: " << backendControllers.size();

        auto jammerProviders = std::make_shared<JammerProviderPtrs>();
        auto sensorProviders = std::make_shared<SensorProviderPtrs>();
        auto cameraProviders = std::make_shared<CameraProviderPtrs>();
        for (const auto& backendController: backendControllers)
        {
            jammerProviders->push_back(backendController->jammers());
            sensorProviders->push_back(backendController->sensors());
            cameraProviders->push_back(backendController->cameras());
        }

        qRegisterMetaType<mpk::dss::core::JammerBandOption>();
        qRegisterMetaType<mpk::dss::core::JammerMode::Value>();

        mpk::dss::core::DeviceDisabledDbHelper disabledDb(dbService);
        const auto dList = disabledDb.queryStates();
        const auto posList = devicePositionStorage->fetchData();
        devicesRegisteredConnect(
            jammerProviders,
            [&dList, &posList](auto& device)
            {
                setDeviceDisabledState<JammerProperties>(device, &dList);
                setDevicePosition<JammerProperties>(device, &posList);
                device.properties().timeoutPersistent().load(device.id());
            });

        auto jammerTimeoutStatusDb =
            std::make_shared<JammerTimeoutStatusDbHelper>(dbService);
        devicesRegisteredConnect(
            sensorProviders,
            [&dList, &posList, &jammerTimeoutStatusDb](auto& device)
            {
                setDeviceDisabledState<SensorProperties>(device, &dList);
                setDevicePosition<SensorProperties>(device, &posList);

                JammerMode::Value mode = JammerMode::Value::UNDEFINED;
                std::chrono::seconds timeout = 0s;
                if (jammerTimeoutStatusDb->queryMode(
                        static_cast<QString>(device.id()), mode, timeout))
                {
                    device.properties().setJammerMode(mode, timeout);
                }
            });

        auto wsPublisher = std::make_shared<WsPublisher>(wsConnectionSettings);

        auto commonApiAdapter = std::make_unique<CommonApiAdapter>(
            *restServer->apiGateway(),
            licenseController.get(),
            sensorProviders,
            jammerProviders);

        auto sensorApiAdapter = std::make_unique<SensorApiAdapter>(
            *restServer->apiGateway(), sensorProviders);

        auto cameraApiAdapter = std::make_unique<CameraApiAdapter>(
            *restServer->apiGateway(), cameraProviders);

        auto jammerApiAdapter = std::make_unique<JammerApiAdapter>(
            *restServer->apiGateway(),
            jammerProviders,
            jammerGroupProvider,
            wsPublisher);

        auto deviceApiAdapter = std::make_unique<DeviceApiAdapter>(
            *restServer->apiGateway(), sensorProviders, jammerProviders);

        auto trackController = std::make_unique<TrackController>();
        for (const auto& backendController: backendControllers)
        {
            connectTrackController(
                backendController.get(),
                trackController.get(),
                licenseController.get());
        }

        auto* targetMinPriority = std::getenv("TARGET_MIN_PRIORITY");
        auto priority = AlarmLevel::NONE;
        if (nullptr == targetMinPriority)
        {
            LOG_FATAL
                << "Missing TARGET_MIN_PRIORITY environment variable, defaulting to NONE";
        }
        else
        {
            QString priorityStr =
                QString::fromUtf8(targetMinPriority).toUpper();
            try
            {
                priority = AlarmLevel::fromString(priorityStr);
                LOG_INFO << "TARGET_MIN_PRIORITY set to: "
                         << AlarmLevel::toString(priority);
            }
            catch (...)
            {
                LOG_WARNING
                    << "Invalid TARGET_MIN_PRIORITY value: " << priorityStr
                    << "defaulting to NONE";
                priority = AlarmLevel::NONE;
            }

            priority = AlarmLevel::fromString(priorityStr);
        }

        auto targetController =
            std::make_unique<TargetController>(trackController.get(), priority);
        auto targetNotifier =
            std::make_unique<TargetNotifier>(targetController.get());

        auto targetApiAdapter = std::make_unique<TargetApiAdapter>(
            *restServer->apiGateway(), targetController.get());

        auto sensorNotifier = std::make_unique<SensorNotifier>(sensorProviders);
        auto jammerNotifier = std::make_unique<JammerNotifier>(jammerProviders);

        connectWSPublisher(
            wsPublisher.get(),
            sensorNotifier.get(),
            jammerNotifier.get(),
            targetNotifier.get(),
            licenseController.get());

        wsPublisher->run();

        auto rc = application.exec();
        LOG_INFO << "Exiting";
        return rc;
    }
    catch (const mpk::dss::backend::exception::MissingParser& ex)
    {
        LOG_DEBUG << ex.what();
        LOG_FATAL << "Missing parser for backend with key: "
                  << ::exception::info<::exception::KeyInfo>(
                         ex, std::string{"emptyKey"})
                  << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (const mpk::dss::backend::exception::InvalidConfigValue& ex)
    {
        LOG_DEBUG << ex.what();
        LOG_FATAL << "Missing config environment variable: "
                  << ::exception::info<::exception::ExceptionInfo>(
                         ex, std::string{"Empty"})
                  << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (const ::exception::InvalidValue& ex)
    {
        LOG_DEBUG << ex.what();
        LOG_FATAL << "Invalid value : "
                  << ::exception::info<::exception::ExceptionInfo>(
                         ex, std::string{"Empty"})
                  << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (const mpk::dss::backend::exception::InvalidConfig& ex)
    {
        LOG_DEBUG << ex.what();
        LOG_FATAL << "Invalid configuration: "
                  << ::exception::info<::exception::ExceptionInfo>(
                         ex, std::string{"no info"})
                  << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        LOG_FATAL
            << "Unhandled exception in application main loop: " << ex.what()
            << ". Exiting";
        return EXIT_FAILURE;
    }
    catch (...)
    {
        LOG_FATAL << "Unhandled exception in application main loop. Exiting";
        return EXIT_FAILURE;
    }
}
