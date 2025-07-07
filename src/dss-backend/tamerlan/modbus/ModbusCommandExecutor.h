#pragma once

#include "dss-backend/tamerlan/modbus/ModbusServerAddress.h"

#include "gsl/pointers"

#include <memory>

#include <QObject>

namespace mpk::dss::backend::tamerlan
{

class ModbusConnection;

class ModbusCommandExecutor : public QObject
{
    Q_OBJECT

public:
    explicit ModbusCommandExecutor(
        std::unique_ptr<ModbusConnection> connection,
        QObject* parent = nullptr);
    ~ModbusCommandExecutor() override;

    using Address = ModbusServerAddress;
    using CommandId = QUuid;

    virtual void readInputRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        Address serverAddress) = 0;
    virtual void readHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        Address serverAddress) = 0;
    virtual void readCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint8_t* destination,
        Address serverAddress) = 0;

    virtual void writeHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint16_t* source,
        Address serverAddress) = 0;
    virtual void writeCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint8_t* source,
        Address serverAddress) = 0;

    virtual void setDisabled(bool disabled);

    [[nodiscard]] gsl::not_null<const ModbusConnection*> constConnection() const;

signals:
    void succeeded(CommandId commandId);
    void failed(CommandId commandId, QString errorStr);

protected:
    [[nodiscard]] gsl::not_null<ModbusConnection*> connection();

private:
    std::unique_ptr<ModbusConnection> m_connection;
};

} // namespace mpk::dss::backend::tamerlan
