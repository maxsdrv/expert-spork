#pragma once

#include "ModbusServerAddress.h"

#include "dss-common/config/ConnectionSettings.h"

#include <modbus/modbus.h>

#include <QHostInfo>

#include <chrono>
#include <string>

namespace mpk::dss::backend::tamerlan
{

struct ModbusRtuSettings
{
    std::string name;
    int baudRate;
    char parity;
    int dataBits;
    int stopBits;
};

class ModbusConnection
{
public:
    ModbusConnection(
        const config::ConnectionSettings& settings,
        std::chrono::milliseconds timeout);
    ModbusConnection(
        const ModbusRtuSettings& settings, std::chrono::milliseconds timeout);

    void readInputRegisters(
        int address,
        int numRegs,
        uint16_t* destination,
        ModbusServerAddress serverAddress);
    void readHoldingRegisters(
        int address,
        int numRegs,
        uint16_t* destination,
        ModbusServerAddress serverAddress);
    void readCoils(
        int address,
        int numRegs,
        uint8_t* destination,
        ModbusServerAddress serverAddress);

    void writeHoldingRegister(
        int address, uint16_t value, ModbusServerAddress serverAddress);
    void writeHoldingRegisters(
        int address,
        int numRegs,
        const uint16_t* data,
        ModbusServerAddress serverAddress);
    void writeCoil(int address, int status, ModbusServerAddress serverAddress);
    void writeCoils(
        int address,
        int numRegs,
        const uint8_t* data,
        ModbusServerAddress serverAddress);

    void setDisabled(bool disabled);

    [[nodiscard]] QString address() const;

private:
    void init();
    void setServerAddress(ModbusServerAddress value);
    void tryConnectOrThrow();
    void processModbusError();
    static bool isErrorAvailableForRetry();
    void createTcpContext(const QHostInfo& hostInfo, int port);

private:
    modbus_t* m_ctx = nullptr;
    QString m_address;
    std::optional<config::ConnectionSettings> m_tcpConnectionSettings =
        std::nullopt;
    std::chrono::milliseconds m_timeout;
    bool m_connected = false;
    bool m_disabled = false;
};

} // namespace mpk::dss::backend::tamerlan
