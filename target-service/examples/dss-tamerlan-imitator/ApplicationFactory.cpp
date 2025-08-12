#include "ApplicationFactory.h"
#include "CommandLineParser.h"
#include "JammerChannelParameters.h"
#include "JammerModel.h"
#include "ModbusServerController.h"
#include "ModbusTcpServer.h"
#include "SensorModel.h"

#include "modbus/device/GeoDataModbus.h"
#include "modbus/device/NetworkDataModbus.h"

#include "dss-backend/tamerlan/modbus/Protocol.h"
#include "dss-common/modbus/connectiondetails.h"

#include <QNetworkInterface>

#include <arpa/inet.h>

namespace mpk::dss::imitator::tamerlan
{

ApplicationFactory::ApplicationFactory(int argc, char** argv, QObject* parent) :
  QObject(parent),
  m_cmdLineParser{std::make_unique<CommandLineParser>(argc, argv)}
{
    QObject::connect(
        m_cmdLineParser.get(),
        &CommandLineParser::helpHasBeenDisplayed,
        this,
        &ApplicationFactory::exit);
}

ApplicationFactory::~ApplicationFactory() = default;

std::unique_ptr<ModbusServer> ApplicationFactory::createTcpServer(
    const ConnectionDetails& cd)
{
    namespace proto = mpk::dss::backend::tamerlan::proto;
    auto server = std::make_unique<ModbusTcpServer>(
        cd.TcpParams.IPAddress, cd.TcpParams.ServicePort);

    std::vector<int> serverAddresses;
    auto sensorsParameters = m_cmdLineParser->getSensorParameters();
    for (const auto& sensorParameters: sensorsParameters)
    {
        auto units =
            sensor::Model::createModbusDataUnits(sensorParameters.channels);
        server->registerModbusDataUnits(
            sensorParameters.serverAddress, ServiceRole::SensorImitator, units);
        serverAddresses.push_back(sensorParameters.serverAddress);
    }
    auto jammersParameters = m_cmdLineParser->getJammerParameters();
    for (const auto& jammerParameters: jammersParameters)
    {
        auto units =
            jammer::Model::createModbusDataUnits(jammerParameters.channels);
        server->registerModbusDataUnits(
            jammerParameters.serverAddress, ServiceRole::JammerImitator, units);
        serverAddresses.push_back(jammerParameters.serverAddress);
    }

    for (const auto serverAddress: serverAddresses)
    {
        server->setData(
            serverAddress,
            QModbusDataUnit::HoldingRegisters,
            proto::ModbusParams::address,
            serverAddress);
        auto networkData = NetworkDataModbus{
            .networkAddress =
                inet_addr((cd.TcpParams.IPAddress.toStdString().c_str()))};
        auto networkDataUnit = QModbusDataUnit{
            QModbusDataUnit::HoldingRegisters,
            proto::NetworkData::address,
            proto::NetworkData::size};
        networkData.store(networkDataUnit);
        server->setData(serverAddress, networkDataUnit);
        server->setData(
            serverAddress,
            QModbusDataUnit::HoldingRegisters,
            proto::ServicePort::address,
            cd.TcpParams.ServicePort);
        auto geoDataUnit = QModbusDataUnit{
            QModbusDataUnit::HoldingRegisters,
            proto::GeoData::address,
            proto::GeoData::size};
        auto locationModbus = m_cmdLineParser->getGeolocation();
        locationModbus.store(geoDataUnit);
        server->setData(serverAddress, geoDataUnit);
    }

    return server;
}

std::unique_ptr<ModbusServer> ApplicationFactory::createSerialServer()
{
    // TODO
    return nullptr;
}

std::unique_ptr<ModbusServer> ApplicationFactory::createModbusServer(
    const ConnectionDetails& cd)
{
    std::unique_ptr<ModbusServer> server{nullptr};
    switch (cd.Type)
    {
        case ConnectionType::Tcp: server = createTcpServer(cd); break;
        case ConnectionType::Serial: server = createSerialServer(); break;
    }
    return server;
}

std::unique_ptr<ModbusServerController> ApplicationFactory::
    createModbusServerController()
{
    m_cmdLineParser->run();

    std::unique_ptr<ModbusServerController> modbusServerController{nullptr};
    auto connectionDetails = m_cmdLineParser->getConnectionDetails();
    if (connectionDetails)
    {
        auto modbusServer = createModbusServer(*connectionDetails);
        modbusServerController =
            std::make_unique<ModbusServerController>(std::move(modbusServer));
    }
    return modbusServerController;
}

} // namespace mpk::dss::imitator::tamerlan
