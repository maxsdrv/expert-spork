#include "Defs.h"

#include "CommandType.h"
#include "TestClient.h"
#include "core/proto/JammerData.h"
#include "core/proto/Protocol.h"

#include <boost/program_options.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
int main(int argc, char** argv)
{
    const std::string appName("Drone detector backend cli");
    const std::string descriptionString(
        "Drone detector backend command line interface");
    // clang-format off
    const auto copyrightString =
        std::string("Copyright (C) IRZ, 2023-") + BUILD_YEAR + ", Saint-Petersburg, Russia\n"
        + "Copyright (C) MPK Software, 2023-" + BUILD_YEAR + ", Saint-Petersburg, Russia\n";
    // clang-format on
    const auto versionString = std::string(VERSION) + "-" + REVISION;

    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc(
        descriptionString + "\n" + "Version: " + versionString + "\n\n"
        + "Command types:\n"
        "  " + mpk::drone::commandTypeToString(mpk::drone::CommandType::READ) + "                read device and channel info\n"
        "  " + mpk::drone::commandTypeToString(mpk::drone::CommandType::SET) + "                 set data on service\n"
        "  " + mpk::drone::commandTypeToString(mpk::drone::CommandType::UPDATE_SOFTWARE) + "     update service software\n"
        "  " + mpk::drone::commandTypeToString(mpk::drone::CommandType::UPDATE_LICENSE) + "      update license\n\n" + "Allowed options");
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    desc.add_options()
        ("help", "display help message")
        ("device_type", po::value<std::string>()->default_value("sensor"), "device type ('sensor' or 'jammer')")
        ("count", po::value<int>()->default_value(1), "request count")
        ("interval", po::value<int>()->default_value(1000), "request interval (ms)")
        ("type", po::value<std::string>()->default_value("tcp"), "modbus connection type")
        ("modbus_timeout", po::value<int>()->default_value(500), "modbus timeout (in milliseconds)")
        ("tcp", po::value<std::string>()->default_value("127.0.0.1:10530:10"), "detector tcp address")
        ("serial", po::value<std::string>()->default_value("/dev/pts/3:115200:N:8:1:10"), "serial port settings")
        ("file", po::value<std::string>(), "file to send")
        ("channel", po::value<int>()->default_value(1), "index of channel to read (1-4 for sensor, 1-16 for jammer)")
        ("modbus_address", po::value<uint16_t>(), "modbus address to set")
        ("ip_address", po::value<std::string>(), "ip address to set (only for 'sensor' device type)")
        ("netmask", po::value<std::string>(), "netmask to set (only for 'sensor' device type)")
        ("gateway", po::value<std::string>(), "gateway to set (only for 'sensor' device type)")
        ("dns", po::value<std::string>(), "DNS to set (only for 'sensor' device type)")
        ("use_dhcp", po::value<bool>(), "use DHCP (only for 'sensor' device type)")
        ("use_dns_from_dhcp", po::value<bool>(), "use DNS from DHCP (only for 'sensor' device type)")
        ("service_port", po::value<int>(), "service port to set (only for 'sensor' device type)")
        ("coordinates", po::value<std::string>(), "coordinates to set ('latitude:longitude', e.g. '60.41:30.15')")
        ("orientation", po::value<float>(), "orientation to set")
        ("direction", po::value<float>(), "direction to set")
        ("work_angle", po::value<float>(), "work angle to set")
        ("distance", po::value<float>(), "distance to set")
        ("min_voltage", po::value<float>(), "min voltage to set (only for 'jammer' device type)")
        ("max_voltage", po::value<float>(), "max voltage to set (only for 'jammer' device type)")
        ("max_temperature", po::value<float>(), "max temperature to set (only for 'jammer' device type)")
        ("channels_delay", po::value<float>(), "channels delay to set (only for 'jammer' device type)")
        ("max_time_current", po::value<float>(), "max time (for current exceeding max value) to set (only for 'jammer' device type)")
        ("max_time_voltage", po::value<float>(), "max time (for voltage below min value) to set (only for 'jammer' device type)")
        ("jammer_addresses", po::value<std::string>(), "jammer addresses to set (list of values separated by ',', e.g. '15,64,162') (only for 'sensor' device type)")
        ("jammer_mode", po::value<uint16_t>(), "jammer mode to set (only for 'sensor' device type)")
        ("jammer_timeout", po::value<uint16_t>(), "jammer timeout to set (only for 'sensor' device type)")
        ("emission_all", po::value<bool>(), "emission for all channels (only for 'jammer' device type)")
        ("emission", po::value<bool>(), "emission (only for 'jammer' device type)")
        ("max_current", po::value<float>(), "max current to set (only for 'jammer' device type)")
        ("max_impulse_current", po::value<float>(), "max impulse current to set (only for 'jammer' device type)");
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    // clang-format on

    po::variables_map vm;
    mpk::drone::CommandType commandType = mpk::drone::CommandType::READ;
    try
    {
        po::command_line_parser parser(argc, argv);
        parser.options(desc);
        auto parsed_options = parser.allow_unregistered().run();
        auto unrecognized = po::collect_unrecognized(
            parsed_options.options, po::include_positional);
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (unrecognized.empty()
            || std::string(argv[1]) != *unrecognized.begin())
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        {
            std::cout << "No command type specified" << std::endl;
            std::cout << std::endl << desc << std::endl;
            return EXIT_FAILURE;
        }
        commandType = mpk::drone::commandTypeFromString(*unrecognized.begin());
        if (unrecognized.size() > 1)
        {
            std::cout << "Unknown option: " << *(unrecognized.begin() + 1)
                      << std::endl;
            std::cout << std::endl << desc << std::endl;
            return EXIT_FAILURE;
        }
        po::store(parsed_options, vm);
        po::notify(vm);
    }
    catch (const po::error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help") > 0)
    {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    auto requestCount = 1;
    auto countIt = vm.find("count");
    if (countIt != vm.end())
    {
        requestCount = countIt->second.as<int>();
    }

    std::string deviceType("sensor");
    auto deviceTypeIt = vm.find("device_type");
    if (deviceTypeIt != vm.end())
    {
        deviceType = deviceTypeIt->second.as<std::string>();
    }

    if (commandType == mpk::drone::CommandType::SET)
    {
        if ((deviceType == "sensor"
             && (vm.find("min_voltage") != vm.end()
                 || vm.find("max_voltage") != vm.end()
                 || vm.find("max_temperature") != vm.end()
                 || vm.find("channels_delay") != vm.end()
                 || vm.find("max_time_current") != vm.end()
                 || vm.find("emission_all") != vm.end()
                 || vm.find("emission") != vm.end()
                 || vm.find("max_current") != vm.end()
                 || vm.find("max_impulse_current") != vm.end()
                 || vm.find("max_time_voltage") != vm.end()))
            || (deviceType == "jammer"
                && (vm.find("ip_address") != vm.end()
                    || vm.find("netmask") != vm.end()
                    || vm.find("gateway") != vm.end()
                    || vm.find("dns") != vm.end()
                    || vm.find("use_dhcp") != vm.end()
                    || vm.find("use_dns_from_dhcp") != vm.end()
                    || vm.find("service_port") != vm.end()
                    || vm.find("jammer_addresses") != vm.end()
                    || vm.find("jammer_mode") != vm.end()
                    || vm.find("jammer_timeout") != vm.end())))
        {
            std::cout << "Unsupported option for device type '" << deviceType
                      << "'" << std::endl;
            std::cout << std::endl << desc << std::endl;
            return EXIT_FAILURE;
        }
    }

    using namespace std::chrono_literals;
    auto requestInterval = 1000ms;
    auto intervalIt = vm.find("interval");
    if (intervalIt != vm.end())
    {
        requestInterval =
            std::chrono::milliseconds(intervalIt->second.as<int>());
    }

    mpk::drone::TestClient::ModbusType modbusType = mpk::drone::TestClient::TCP;
    std::string description;

    auto typeIt = vm.find("type");
    if (typeIt->second.as<std::string>() == "tcp")
    {
        modbusType = mpk::drone::TestClient::TCP;
        auto addressIt = vm.find("tcp");
        if (addressIt != vm.end())
        {
            description = addressIt->second.as<std::string>();
        }
    }
    else if (typeIt->second.as<std::string>() == "serial")
    {
        modbusType = mpk::drone::TestClient::SERIAL;
        auto serialIt = vm.find("serial");
        if (serialIt != vm.end())
        {
            description = serialIt->second.as<std::string>();
        }
    }
    auto timeoutIt = vm.find("modbus_timeout");

    mpk::drone::TestClient testClient(
        modbusType, description, timeoutIt->second.as<int>());

    auto channelIndex = 0;
    auto channelIt = vm.find("channel");
    if (channelIt != vm.end())
    {
        channelIndex = channelIt->second.as<int>() - 1;
        if (channelIndex < 0
            || (deviceType == "sensor"
                && channelIndex
                       >= mpk::drone::core::proto::maxSensorChannelsCount)
            || (deviceType == "jammer"
                && channelIndex
                       >= mpk::drone::core::proto::maxJammerChannelsCount))
        {
            std::cout << "Invalid channel index: '" << (channelIndex + 1) << "'"
                      << std::endl;
            std::cout << std::endl << desc << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (commandType == mpk::drone::CommandType::READ)
    {
        try
        {
            for (auto i = 0; i < requestCount || requestCount == 0; ++i)
            {
                auto timePoint =
                    std::chrono::system_clock::now() + requestInterval;
                std::cout << "****** Iteration " << i << " ******" << std::endl;
                if (deviceType == "sensor")
                {
                    testClient.getDeviceData(true);
                    testClient.getSensorChannel(channelIndex);
                    testClient.getJammerData();
                }
                else if (deviceType == "jammer")
                {
                    testClient.getDeviceData(false);
                    testClient.getJammerChannel(channelIndex);
                }
                std::flush(std::cout);
                std::this_thread::sleep_until(timePoint);
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Read failure: " << ex.what() << "\n";
        }
        catch (...)
        {
            std::cerr << "Read failure: internal error.\n";
        }
    }
    else if (commandType == mpk::drone::CommandType::SET)
    {
        try
        {
            auto modbusAddressIt = vm.find("modbus_address");
            if (modbusAddressIt != vm.end())
            {
                testClient.setModbusAddress(
                    modbusAddressIt->second.as<uint16_t>());
            }
            if (deviceType == "sensor")
            {
                auto networkAddressIt = vm.find("ip_address");
                if (networkAddressIt != vm.end())
                {
                    testClient.setNetworkAddress(
                        networkAddressIt->second.as<std::string>());
                }
                auto netmaskIt = vm.find("netmask");
                if (netmaskIt != vm.end())
                {
                    testClient.setNetmask(netmaskIt->second.as<std::string>());
                }
                auto gatewayIt = vm.find("gateway");
                if (gatewayIt != vm.end())
                {
                    testClient.setGateway(gatewayIt->second.as<std::string>());
                }
                auto dnsIt = vm.find("dns");
                if (dnsIt != vm.end())
                {
                    testClient.setDns(dnsIt->second.as<std::string>());
                }
                auto useDhcpIt = vm.find("use_dhcp");
                if (useDhcpIt != vm.end())
                {
                    testClient.setUseDhcp(useDhcpIt->second.as<bool>());
                }
                auto useDnsFromDhcpIt = vm.find("use_dns_from_dhcp");
                if (useDnsFromDhcpIt != vm.end())
                {
                    testClient.setUseDnsFromDhcp(
                        useDnsFromDhcpIt->second.as<bool>());
                }
                auto portIt = vm.find("service_port");
                if (portIt != vm.end())
                {
                    testClient.setNetworkPort(portIt->second.as<int>());
                }
            }
            auto coordinatesIt = vm.find("coordinates");
            if (coordinatesIt != vm.end())
            {
                auto coordString = coordinatesIt->second.as<std::string>();
                auto pos = coordString.find_first_of(':');
                if (pos == std::string::npos)
                {
                    throw std::runtime_error(
                        std::string("Failed to parse coordinates <")
                        + coordString + ">");
                }
                auto latitude = std::stof(coordString.substr(0, pos));
                auto longitude =
                    std::stof(coordString.substr(pos + 1, coordString.size()));
                testClient.setCoordinates(latitude, longitude);
            }
            auto orientationIt = vm.find("orientation");
            if (orientationIt != vm.end())
            {
                testClient.setOrientation(orientationIt->second.as<float>());
            }

            if (deviceType == "sensor")
            {
                auto directionIt = vm.find("direction");
                if (directionIt != vm.end())
                {
                    testClient.setDirection(
                        channelIndex, directionIt->second.as<float>());
                }
                auto workAngleIt = vm.find("work_angle");
                if (workAngleIt != vm.end())
                {
                    testClient.setWorkAngle(
                        channelIndex, workAngleIt->second.as<float>());
                }
                auto distanceIt = vm.find("distance");
                if (distanceIt != vm.end())
                {
                    testClient.setDistance(
                        channelIndex, distanceIt->second.as<float>());
                }
                auto jammerAddressesIt = vm.find("jammer_addresses");
                if (jammerAddressesIt != vm.end())
                {
                    auto addresses =
                        jammerAddressesIt->second.as<std::string>();
                    std::array<
                        uint16_t,
                        mpk::drone::core::JammerModbus::jammersCount>
                        addressesArray = {0};
                    std::size_t pos = 0U;
                    int index = 0;
                    try
                    {
                        while (pos != addresses.npos)
                        {
                            auto addressStr = static_cast<std::string>(
                                addresses.substr(pos == 0 ? pos : (pos + 1), 4));
                            addressesArray.at(index++) =
                                static_cast<uint16_t>(std::stoi(addressStr));
                            pos = addresses.find_first_of(',', pos + 1);
                        }
                    }
                    catch (const std::exception& /*ex*/)
                    {
                        std::cout << "Invalid jammer addresses string: '"
                                  << addresses << "'" << std::endl;
                        std::cout << std::endl << desc << std::endl;
                        return EXIT_FAILURE;
                    }

                    testClient.setJammerAddresses(addressesArray);
                }
                auto jammerModeIt = vm.find("jammer_mode");
                if (jammerModeIt != vm.end())
                {
                    testClient.setJammerMode(
                        jammerModeIt->second.as<uint16_t>());
                }
                auto jammerTimeoutIt = vm.find("jammer_timeout");
                if (jammerTimeoutIt != vm.end())
                {
                    testClient.setJammerTimeout(
                        jammerTimeoutIt->second.as<uint16_t>());
                }
            }
            else if (deviceType == "jammer")
            {
                auto minVoltageIt = vm.find("min_voltage");
                if (minVoltageIt != vm.end())
                {
                    testClient.setMinVoltage(minVoltageIt->second.as<float>());
                }
                auto maxVoltageIt = vm.find("max_voltage");
                if (maxVoltageIt != vm.end())
                {
                    testClient.setMaxVoltage(maxVoltageIt->second.as<float>());
                }
                auto maxTemperatureIt = vm.find("max_temperature");
                if (maxTemperatureIt != vm.end())
                {
                    testClient.setMaxTemperature(
                        maxTemperatureIt->second.as<float>());
                }
                auto channelsDelayIt = vm.find("channels_delay");
                if (channelsDelayIt != vm.end())
                {
                    testClient.setChannelsDelay(
                        channelsDelayIt->second.as<float>());
                }
                auto maxTimeCurrentIt = vm.find("max_time_current");
                if (maxTimeCurrentIt != vm.end())
                {
                    testClient.setMaxTimeCurrent(
                        maxTimeCurrentIt->second.as<float>());
                }
                auto maxTimeVoltageIt = vm.find("max_time_voltage");
                if (maxTimeVoltageIt != vm.end())
                {
                    testClient.setMaxTimeVoltage(
                        maxTimeVoltageIt->second.as<float>());
                }
                auto jammerEmissionAllIt = vm.find("emission_all");
                if (jammerEmissionAllIt != vm.end())
                {
                    testClient.setEmissionAll(
                        jammerEmissionAllIt->second.as<bool>());
                }
                auto jammerEmissionIt = vm.find("emission");
                if (jammerEmissionIt != vm.end())
                {
                    testClient.setJammerEmission(
                        channelIndex, jammerEmissionIt->second.as<bool>());
                }
                auto maxCurrentIt = vm.find("max_current");
                if (maxCurrentIt != vm.end())
                {
                    testClient.setMaxCurrent(
                        channelIndex, maxCurrentIt->second.as<float>());
                }
                auto maxImpulseCurrentIt = vm.find("max_impulse_current");
                if (maxImpulseCurrentIt != vm.end())
                {
                    testClient.setMaxImpulseCurrent(
                        channelIndex, maxImpulseCurrentIt->second.as<float>());
                }
            }
            std::cout << std::endl << "**** After 'set' commands:" << std::endl;
            if (deviceType == "sensor")
            {
                testClient.getDeviceData(true);
                testClient.getSensorChannel(channelIndex);
                testClient.getJammerData();
            }
            else if (deviceType == "jammer")
            {
                testClient.getDeviceData(false);
                testClient.getJammerChannel(channelIndex);
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Set failure: " << ex.what() << "\n";
            return EXIT_FAILURE;
        }
        catch (...)
        {
            std::cerr << "Set failure: internal error.\n";
            return EXIT_FAILURE;
        }
    }
    else if (
        commandType == mpk::drone::CommandType::UPDATE_SOFTWARE
        || commandType == mpk::drone::CommandType::UPDATE_LICENSE)
    {
        try
        {
            auto fileIt = vm.find("file");
            if (fileIt != vm.end())
            {
                auto fileName = fileIt->second.as<std::string>();
                std::cout << "****** Sending file " << fileName << " ******"
                          << std::endl;
                testClient.sendFile(fileName);
                commandType == mpk::drone::CommandType::UPDATE_SOFTWARE ?
                    testClient.setSoftwareUpdate() :
                    testClient.setLicenseUpdate();
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Update failure: " << ex.what() << "\n";
            return EXIT_FAILURE;
        }
        catch (...)
        {
            std::cerr << "Update failure: internal error.\n";
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
