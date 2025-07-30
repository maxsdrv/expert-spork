#include "CommandLineParser.h"

#include "dss-backend/tamerlan/modbus/Protocol.h"

#include "log/Log2.h"

#include <QStringList>

#include "gsl/assert"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <regex>

namespace mpk::dss::imitator::tamerlan
{

namespace
{
constexpr auto helpTag = "help";
constexpr auto modbusTypeTag = "modbus_type";
constexpr auto tcpTag = "tcp";
constexpr auto serialTag = "serial";
constexpr auto locationTag = "location";
constexpr auto azimuthTag = "azimuth";
constexpr auto sensorsTag = "sensors";
constexpr auto jammersTag = "jammers";

const auto defaultLocation = std::string{"30.23,60.23,10.10"};
} // namespace

CommandLineParser::CommandLineParser(int argc, char** argv, QObject* parent) :
  QObject(parent),
  m_parser{argc, argv},
  m_connectionDetails{std::make_optional(ConnectionDetails())}
{
}

void CommandLineParser::run()
{
    using namespace boost::program_options;

    options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        (helpTag, "display help message")
        (locationTag, value<std::string>()->default_value(defaultLocation), "set location in format: \'30.23 60.23 10.10\' or 30.23,60.23,10.10")
        (azimuthTag, value<std::string>(), "set azimuth")
        (modbusTypeTag, value<std::string>()->default_value(tcpTag), "set modbus type to one of the possible types: tcp or serial")
        (sensorsTag, value<std::string>(), "add sensors in format '<sensor1>:<sensor2>', where each sensor is in format '<server_address>,<channels>', where 'server_address' is modbus server instance number and 'channels' is list of channel statuses (with one of the possible values: noalarm, alarm, error) and workzone params (direction, angle, distance). Example: '10,noalarm,60,30,100,alarm,45,90,200,noalarm,0,30,50'")
        (jammersTag, value<std::string>(), "add jammers in format '<jammer1>:<jammer2>', where each jammer is in format '<server_address>,<channels>', where 'server_address' is modbus server instance number and 'channels' is list of channel names. Example: '20,channel_01,channel_02'");
    // clang-format on

    m_parser.options(desc);
    auto parsedOptions = m_parser.run();
    variables_map vm;
    store(parsedOptions, vm);
    parseHelpOption(vm, desc);
    parseModbusTypeOption(vm);
    parseGeolocation(vm);
    parseAzimuth(vm);
    parseSensors(vm);
    parseJammers(vm);
    notify(vm);
}

std::optional<ConnectionDetails> CommandLineParser::getConnectionDetails() const
{
    return m_connectionDetails;
}

GeoDataModbus CommandLineParser::getGeolocation() const
{
    return m_geolocation;
}

std::vector<SensorParameters> CommandLineParser::getSensorParameters() const
{
    return m_sensorParameters;
}

std::vector<JammerParameters> CommandLineParser::getJammerParameters() const
{
    return m_jammerParameters;
}

void CommandLineParser::parseHelpOption(
    const boost::program_options::variables_map& vm,
    const boost::program_options::options_description& desc)
{
    if (vm.count(helpTag) != 0)
    {
        std::cout << desc << std::endl;
        emit helpHasBeenDisplayed();
    }
}

void CommandLineParser::parseModbusTypeOption(
    const boost::program_options::variables_map& vm)
{
    if (vm.count(modbusTypeTag) != 0)
    {
        const auto& str = vm[modbusTypeTag].as<std::string>();
        auto regex = std::regex{R"([\s,]+)"};
        auto it = std::sregex_token_iterator{str.begin(), str.end(), regex, -1};
        if (std::distance(it, std::sregex_token_iterator{}) == 2)
        {
            parseModbusType(*it++);
            parseModbusParams(*it++);
        }
    }
}

void CommandLineParser::parseModbusType(const std::string& str)
{
    if (!m_connectionDetails.has_value())
    {
        throw std::runtime_error("Missing connection details");
    }
    if (str == tcpTag)
    {
        m_connectionDetails->Type = ConnectionType::Tcp;
    }
    else if (str == serialTag)
    {
        m_connectionDetails->Type = ConnectionType::Serial;
    }
    else
    {
        throw std::runtime_error("Unknown modbus connection type " + str);
    }
}

void CommandLineParser::parseModbusParams(const std::string& str)
{
    if (!m_connectionDetails.has_value())
    {
        throw std::runtime_error("Missing connection details");
    }
    switch (m_connectionDetails->Type)
    {
        case ConnectionType::Tcp:
            m_connectionDetails->TcpParams = parseTcpParams(str);
            break;
    case ConnectionType::Serial:
            m_connectionDetails->SerialParams = parseSerialParams(str);
        break;
    }
}

TcpConnectionParams CommandLineParser::parseTcpParams(const std::string& str)
{
    if (!str.empty())
    {
        auto regex = std::regex{R"([:]+)"};
        auto it = std::sregex_token_iterator{str.begin(), str.end(), regex, -1};
        if (std::distance(it, std::sregex_token_iterator{}) == 2)
        {
            auto address = QString::fromStdString(*it++);
            auto port =
                static_cast<quint16>(QString::fromStdString(*it++).toUShort());
            return TcpConnectionParams{
                .ServicePort = port, .IPAddress = address};
        }
    }
    return TcpConnectionParams{};
}

SerialConnectionParams CommandLineParser::parseSerialParams(
    const std::string& str)
{
    if (!str.empty())
    {
        // ToDo
    }
    return SerialConnectionParams{};
}

void CommandLineParser::parseGeolocation(
    const boost::program_options::variables_map& vm)
{
    if (vm.contains(locationTag))
    {
        const auto& str = vm[locationTag].as<std::string>();
        auto regex = std::regex{R"([\s,]+)"};
        auto it = std::sregex_token_iterator{str.begin(), str.end(), regex, -1};
        if (std::distance(it, std::sregex_token_iterator{}) >= 2)
        {
            m_geolocation.latitude = std::stof(*it++);
            m_geolocation.longitude = std::stof(*it++);
        }
    }
}

void CommandLineParser::parseAzimuth(
    const boost::program_options::variables_map& vm)
{
    if (vm.contains(azimuthTag))
    {
        const auto& str = vm[azimuthTag].as<std::string>();
        m_geolocation.orientation = std::stof(str);
    }
}

void CommandLineParser::parseSensors(
    const boost::program_options::variables_map& vm)
{
    if (vm.contains(sensorsTag))
    {
        if (!addSensor(vm[sensorsTag].as<std::string>()))
        {
            throw std::runtime_error("Incorrect sensor format\n");
        }
    }
}

void CommandLineParser::parseJammers(
    const boost::program_options::variables_map& vm)
{
    if (vm.contains(jammersTag))
    {
        if (!addJammer(vm[jammersTag].as<std::string>()))
        {
            throw std::runtime_error("Incorrect jammer format\n");
        }
    }
}

bool CommandLineParser::addSensor(const std::string& opt)
{
    using namespace mpk::dss::backend::tamerlan;

    const int channelParamsCount = 4;
    auto sensors = QString::fromStdString(opt).split(":");
    for (const auto& sensor: sensors)
    {
        auto sensorParamsStr = sensor.split(",");
        if (sensorParamsStr.size() < 1 + channelParamsCount)
        {
            LOG_ERROR << "Incorrect sensor format: " << sensor.toStdString();
            return false;
        }
        auto serverAddress = sensorParamsStr.at(0).toInt();
        auto sensorParams = SensorParameters{
            .serverAddress = serverAddress,
            .channels = SensorChannelsParameters{}};
        auto currentIndex = 1;
        while (currentIndex < sensorParamsStr.size())
        {
            if (sensorParams.channels.size() >= proto::maxSensorChannelsCount)
            {
                return true;
            }
            try
            {
                sensorParams.channels.push_back(SensorChannelParameter{
                    .status = str2ChannelStatus(
                        sensorParamsStr.at(currentIndex++).toStdString()),
                    .channelZone = {
                        .direction = std::stof(
                            sensorParamsStr.at(currentIndex++).toStdString()),
                        .workAngle = std::stof(
                            sensorParamsStr.at(currentIndex++).toStdString()),
                        .distance = std::stof(
                            sensorParamsStr.at(currentIndex++).toStdString())}});
            }
            catch (std::runtime_error& e)
            {
                LOG_ERROR << e.what();
                return false;
            }
        }
        m_sensorParameters.emplace_back(sensorParams);
    }
    return true;
}

bool CommandLineParser::addJammer(const std::string& opt)
{
    using namespace mpk::dss::backend::tamerlan;

    auto jammers = QString::fromStdString(opt).split(":");
    for (const auto& jammer: jammers)
    {
        auto jammerParamsStr = jammer.split(",");
        if (jammerParamsStr.size() < 2)
        {
            LOG_ERROR << "Incorrect jammer format: " << jammer.toStdString();
            return false;
        }
        auto serverAddress = jammerParamsStr.at(0).toInt();
        auto jammerParams = JammerParameters{
            .serverAddress = serverAddress,
            .channels = JammerChannelsParameters{}};
        for (int i = 1; i < jammerParamsStr.size(); i++)
        {
            if (jammerParams.channels.size() >= proto::maxJammerChannelsCount)
            {
                return true;
            }
            jammerParams.channels.push_back(JammerChannelParameter{
                .name = jammerParamsStr.at(i).toStdString()});
        }
        m_jammerParameters.emplace_back(jammerParams);
    }
    return true;
}

SensorStatus CommandLineParser::str2ChannelStatus(const std::string& str)
{
    static std::map<std::string, SensorStatus> statusMap{
        {"noalarm", SensorStatus::NoAlarm},
        {"alarm", SensorStatus::Alarm},
        {"error", SensorStatus::Error}};

    auto statusIt = statusMap.find(boost::to_lower_copy<std::string>(str));
    if (statusIt == statusMap.end())
    {
        throw std::runtime_error("Impossible channel status \'" + str + "\'");
    }
    return statusIt->second;
}

} // namespace mpk::dss::imitator::tamerlan
