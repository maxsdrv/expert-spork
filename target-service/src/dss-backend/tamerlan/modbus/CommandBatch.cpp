#include "CommandBatch.h"

#include "ModbusCommandExecutor.h"

#include <QUuid>

namespace mpk::dss::backend::tamerlan
{

CommandBatch::CommandBatch(Commands commands, QObject* parent) :
  QObject(parent), m_commands(std::move(commands))
{
}

CommandBatch::Commands CommandBatch::commands() const
{
    return m_commands;
}

void CommandBatch::execute(gsl::not_null<ModbusCommandExecutor*> executor)
{
    m_running = true;
    for (size_t i = 0; i < m_commands.size(); ++i)
    {
        auto id = QUuid::createUuid();
        connect(
            executor,
            &ModbusCommandExecutor::failed,
            this,
            [this, id](const auto& commandId, const auto& errorStr)
            {
                if (commandId == id)
                {
                    m_running = false;
                    emit failed(errorStr);
                    emit finished();
                }
            },
            Qt::QueuedConnection);
        if (i == m_commands.size() - 1)
        {
            connect(
                executor,
                &ModbusCommandExecutor::succeeded,
                this,
                [this, id](const auto& commandId)
                {
                    if (commandId == id)
                    {
                        m_running = false;
                        emit succeeded();
                        emit finished();
                    }
                },
                Qt::QueuedConnection);
        }

        const auto& command = m_commands.at(i);
        if (command.type == Command::READ_INPUT_REGISTERS)
        {
            executor->readInputRegisters(
                id,
                command.address,
                command.numRegs,
                std::get<uint16_t*>(command.data),
                command.serverAddress);
        }
        else if (command.type == Command::READ_HOLDING_REGISTERS)
        {
            executor->readHoldingRegisters(
                id,
                command.address,
                command.numRegs,
                std::get<uint16_t*>(command.data),
                command.serverAddress);
        }
        else if (command.type == Command::READ_COILS)
        {
            executor->readCoils(
                id,
                command.address,
                command.numRegs,
                std::get<uint8_t*>(command.data),
                command.serverAddress);
        }
        else if (command.type == Command::WRITE_HOLDING_REGISTERS)
        {
            executor->writeHoldingRegisters(
                id,
                command.address,
                command.numRegs,
                std::get<uint16_t*>(command.data),
                command.serverAddress);
        }
        else if (command.type == Command::WRITE_COILS)
        {
            executor->writeCoils(
                id,
                command.address,
                command.numRegs,
                std::get<uint8_t*>(command.data),
                command.serverAddress);
        }
    }
}

bool CommandBatch::running() const
{
    return m_running;
}

} // namespace mpk::dss::backend::tamerlan
