#pragma once

#include "dss-backend/tamerlan/modbus/ModbusCommandExecutor.h"

#include <QObject>
#include <QUuid>

namespace mpk::dss::backend::tamerlan
{

class ModbusConnection;

class SyncModbusCommandExecutor : public ModbusCommandExecutor
{
    Q_OBJECT

public:
    explicit SyncModbusCommandExecutor(
        std::unique_ptr<ModbusConnection> connection,
        QObject* parent = nullptr);

    void readInputRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        Address serverAddress) override;
    void readHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        Address serverAddress) override;
    void readCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint8_t* destination,
        Address serverAddress) override;

    void writeHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint16_t* source,
        Address serverAddress) override;
    void writeCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint8_t* source,
        Address serverAddress) override;
};

} // namespace mpk::dss::backend::tamerlan
