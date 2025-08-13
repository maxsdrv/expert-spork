#include "dss-backend/tamerlan/modbus/SyncModbusCommandExecutor.h"

#include "dss-backend/tamerlan/modbus/ModbusConnection.h"

#include "log/Log2.h"

namespace mpk::dss::backend::tamerlan
{

SyncModbusCommandExecutor::SyncModbusCommandExecutor(
    std::unique_ptr<ModbusConnection> connection, QObject* parent) :
  ModbusCommandExecutor{std::move(connection), parent}
{
}

void SyncModbusCommandExecutor::readInputRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint16_t* destination,
    Address serverAddress)
{
    try
    {
        connection()->readInputRegisters(
            address, numRegs, destination, serverAddress);
        emit succeeded(commandId);
    }
    catch (const std::exception& ex)
    {
        auto errorStr =
            QString("Error while running command: %1").arg(ex.what());
        LOG_ERROR << errorStr.toStdString();
        emit failed(commandId, errorStr);
    }
}

void SyncModbusCommandExecutor::readHoldingRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint16_t* destination,
    Address serverAddress)
{
    try
    {
        connection()->readHoldingRegisters(
            address, numRegs, destination, serverAddress);
        emit succeeded(commandId);
    }
    catch (const std::exception& ex)
    {
        auto errorStr =
            QString("Error while running command: %1").arg(ex.what());
        LOG_ERROR << errorStr.toStdString();
        emit failed(commandId, errorStr);
    }
}

void SyncModbusCommandExecutor::readCoils(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint8_t* destination,
    Address serverAddress)
{
    try
    {
        connection()->readCoils(address, numRegs, destination, serverAddress);
        emit succeeded(commandId);
    }
    catch (const std::exception& ex)
    {
        auto errorStr =
            QString("Error while running command: %1").arg(ex.what());
        LOG_ERROR << errorStr.toStdString();
        emit failed(commandId, errorStr);
    }
}

void SyncModbusCommandExecutor::writeHoldingRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    const uint16_t* source,
    Address serverAddress)
{
    try
    {
        connection()->writeHoldingRegisters(
            address, numRegs, source, serverAddress);
        emit succeeded(commandId);
    }
    catch (const std::exception& ex)
    {
        auto errorStr =
            QString("Error while running command: %1").arg(ex.what());
        LOG_ERROR << errorStr.toStdString();
        emit failed(commandId, errorStr);
    }
}

void SyncModbusCommandExecutor::writeCoils(
    const CommandId& commandId,
    int address,
    int numRegs,
    const uint8_t* source,
    Address serverAddress)
{
    try
    {
        connection()->writeCoils(address, numRegs, source, serverAddress);
        emit succeeded(commandId);
    }
    catch (const std::exception& ex)
    {
        auto errorStr =
            QString("Error while running command: %1").arg(ex.what());
        LOG_ERROR << errorStr.toStdString();
        emit failed(commandId, errorStr);
    }
}

} // namespace mpk::dss::backend::tamerlan
