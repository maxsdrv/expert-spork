#pragma once

#include "JammerChannelParameters.h"
#include "SensorChannelParameters.h"
#include "ServiceRole.h"
#include "modbus/device/GeoDataModbus.h"

#include "dss-common/modbus/connectiondetails.h"

#include <boost/program_options.hpp>

#include <optional>

namespace mpk::dss::imitator::tamerlan
{

struct SensorParameters
{
    int serverAddress;
    SensorChannelsParameters channels;
};

struct JammerParameters
{
    int serverAddress;
    JammerChannelsParameters channels;
};

class CommandLineParser : public QObject
{
    Q_OBJECT

public:
    CommandLineParser(int argc, char** argv, QObject* parent = nullptr);
    void run();
    std::optional<ConnectionDetails> getConnectionDetails() const;
    GeoDataModbus getGeolocation() const;
    std::vector<SensorParameters> getSensorParameters() const;
    std::vector<JammerParameters> getJammerParameters() const;

signals:
    void helpHasBeenDisplayed();

private:
    void parseHelpOption(
        const boost::program_options::variables_map&,
        const boost::program_options::options_description& desc);
    void parseModbusTypeOption(const boost::program_options::variables_map&);
    void parseModbusType(const std::string&);
    void parseModbusParams(const std::string&);
    [[nodiscard]] static TcpConnectionParams parseTcpParams(const std::string&);
    [[nodiscard]] static SerialConnectionParams parseSerialParams(
        const std::string&);
    void parseServiceRoleOption(const boost::program_options::variables_map&);
    void parseServerAddress(const boost::program_options::variables_map&);
    void parseGeolocation(const boost::program_options::variables_map&);
    void parseAzimuth(const boost::program_options::variables_map&);
    void parseSensors(const boost::program_options::variables_map&);
    void parseJammers(const boost::program_options::variables_map&);
    bool addSensor(const std::string& opt);
    bool addJammer(const std::string& opt);
    static SensorStatus str2ChannelStatus(const std::string&);

    boost::program_options::command_line_parser m_parser;
    std::optional<ConnectionDetails> m_connectionDetails;
    GeoDataModbus m_geolocation;

    std::vector<SensorParameters> m_sensorParameters;
    std::vector<JammerParameters> m_jammerParameters;
};

} // namespace mpk::dss::imitator::tamerlan
