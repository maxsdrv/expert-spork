#pragma once

#include "ModbusDataUnitsDef.h"
#include "ModbusServer.h"

#include <QModbusPdu>
#include <QString>

#include <modbus/modbus.h>

namespace mpk::dss::imitator::tamerlan
{

using ForwardingTable = std::map<int, int>;

class ModbusTcpServer : public ModbusServer
{
    Q_OBJECT

public:
    ModbusTcpServer(const QString& host, int port, QObject* parent = nullptr);

    Q_INVOKABLE void start() override;
    void stop() override;
    [[nodiscard]] std::vector<int> serverAddresses() const override;
    [[nodiscard]] ServiceRole serviceRole(int serverAddress) const override;
    bool data(int serverAddress, QModbusDataUnit* newData) const override;
    bool data(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16* dataPtr) const override;
    bool setData(int serverAddress, const QModbusDataUnit& newData) override;
    bool setData(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16 newData) override;

    void registerModbusDataUnits(
        int serverAddress, ServiceRole role, const ModbusDataUnits& units);

private:
    modbus_t* m_server = nullptr;
    bool m_stopFlag = false;
    // std::map<int, std::multimap<QModbusDataUnit::RegisterType,
    // QModbusDataUnit>>
    //    m_modbusDataUnitMap;
    struct SingleDeviceData
    {
        ServiceRole role;
        modbus_mapping_t* mapping;
    };
    std::map<int, SingleDeviceData> m_dataMap;
};

} // namespace mpk::dss::imitator::tamerlan
