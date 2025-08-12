#pragma once

#include "CommandLineParser.h"
#include "ModbusDataUnitsDef.h"
#include "ModbusServer.h"
#include "ServiceRole.h"

#include "dss-common/modbus/connectiondetails.h"

#include <QModbusDataUnitMap>
#include <QObject>

#include <memory>

namespace mpk::dss::imitator::tamerlan
{

class ModbusServerController;
class ModbusTcpServer;
class CommandLineParser;

class ApplicationFactory : public QObject
{
    Q_OBJECT

public:
    ApplicationFactory(int argc, char** argv, QObject* parent = nullptr);
    ~ApplicationFactory() override;

    std::unique_ptr<ModbusServerController> createModbusServerController();

signals:
    void exit();

private:
    std::unique_ptr<ModbusServer> createModbusServer(
        const ConnectionDetails& cd);
    std::unique_ptr<ModbusServer> createTcpServer(const ConnectionDetails& cd);
    static std::unique_ptr<ModbusServer> createSerialServer();

    std::unique_ptr<CommandLineParser> m_cmdLineParser;
};

} // namespace mpk::dss::imitator::tamerlan
