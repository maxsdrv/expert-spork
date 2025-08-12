#include "ModbusServerController.h"

#include "exception/General.h"

#include <QtSerialBus/QModbusRtuSerialSlave>
#include <QtSerialBus/QModbusTcpServer>

namespace
{

constexpr auto typeTag = "type";

// TCP parameters
constexpr auto portTag = "port";

// Serial parameters
constexpr auto portNameTag = "portName";
constexpr auto parityTag = "parity";
constexpr auto baudRateTag = "baudRate";
constexpr auto dataBitsTag = "dataBits";
constexpr auto stopBitsTag = "stopBits";
constexpr auto serverAddressTag = "serverAddress";

std::unique_ptr<QModbusServer> createTcpServer(const YAML::Node& description)
{
    auto server = std::make_unique<QModbusTcpServer>();
    server->setConnectionParameter(
        QModbusDevice::NetworkAddressParameter, "0.0.0.0");
    server->setConnectionParameter(
        QModbusDevice::NetworkPortParameter, description[portTag].as<int>());
    return server;
}

std::unique_ptr<QModbusServer> createSerialServer(const YAML::Node& description)
{
    auto server = std::make_unique<QModbusRtuSerialSlave>();
    server->setConnectionParameter(
        QModbusDevice::SerialPortNameParameter,
        QString::fromStdString(description[portNameTag].as<std::string>()));
    server->setConnectionParameter(
        QModbusDevice::SerialParityParameter,
        QString::fromStdString(description[parityTag].as<std::string>()));
    server->setConnectionParameter(
        QModbusDevice::SerialBaudRateParameter,
        description[baudRateTag].as<int>());
    server->setConnectionParameter(
        QModbusDevice::SerialDataBitsParameter,
        description[dataBitsTag].as<int>());
    server->setConnectionParameter(
        QModbusDevice::SerialStopBitsParameter,
        description[stopBitsTag].as<int>());
    server->setServerAddress(description[serverAddressTag].as<int>());
    return server;
}

} // namespace

namespace mpk::drone::core
{

ModbusServerController::ModbusServerController(const YAML::Node& description)
{
    m_creators.insert({ModbusServerType::TCP, createTcpServer});
    m_creators.insert({ModbusServerType::SERIAL, createSerialServer});

    m_type = modbusServerTypeFromString(description[typeTag].as<std::string>());
    auto creatorIt = m_creators.find(m_type);
    if (creatorIt == m_creators.end())
    {
        BOOST_THROW_EXCEPTION(
            exception::General()
            << exception::ExceptionInfo("Unknown modbus server type"));
    }
    m_server = creatorIt->second(description);
}

void ModbusServerController::start()
{
    if (!m_server->connectDevice())
    {
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                "An error occurred while starting modbus server: "
                + m_server->errorString().toStdString()));
    }
}

void ModbusServerController::stop()
{
    m_server->disconnectDevice();
}

gsl::not_null<QModbusServer*> ModbusServerController::server() const
{
    return m_server.get();
}

void ModbusServerController::setPort(int port)
{
    if (m_type == ModbusServerType::TCP)
    {
        if (m_server->connectionParameter(QModbusDevice::NetworkPortParameter)
                .toInt()
            != port)
        {
            stop();
            m_server->setConnectionParameter(
                QModbusDevice::NetworkPortParameter, port);
            start();
        }
    }
}

} // namespace mpk::drone::core
