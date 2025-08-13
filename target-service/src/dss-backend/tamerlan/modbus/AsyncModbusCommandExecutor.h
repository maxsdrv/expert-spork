#pragma once

#include "dss-backend/tamerlan/modbus/ModbusCommandExecutor.h"

#include "dss-core/LockableQueue.h"

#include <QUuid>

#include <atomic>
#include <memory>
#include <mutex>

namespace mpk::dss::backend::tamerlan
{

class ModbusConnection;

class AsyncModbusCommandExecutor : public ModbusCommandExecutor
{
    Q_OBJECT

public:
    explicit AsyncModbusCommandExecutor(
        std::unique_ptr<ModbusConnection> connection,
        QObject* parent = nullptr);

    ~AsyncModbusCommandExecutor() override;

    void readInputRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        ModbusServerAddress serverAddress) override;
    void readHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint16_t* destination,
        ModbusServerAddress serverAddress) override;
    void readCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        uint8_t* destination,
        ModbusServerAddress serverAddress) override;

    void writeHoldingRegisters(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint16_t* source,
        ModbusServerAddress serverAddress) override;
    void writeCoils(
        const CommandId& commandId,
        int address,
        int numRegs,
        const uint8_t* source,
        ModbusServerAddress serverAddress) override;

    void setDisabled(bool disabled) override;

    // TODO: this is crunches
    void lock();
    void unlock();
    void addObject(QObject* object);
    void setDisabled(QObject* object, bool disabled);

signals:
    void wokenUp();
    void fellAsleep();

private slots:
    void process();

private:
    using SafeModbusCall = std::function<void()>;
    using IdCallPair = std::pair<CommandId, SafeModbusCall>;

    void setSleepFlag(bool sleepFlag);
    void pushCall(const IdCallPair& call);

private:
    core::LockableQueue<IdCallPair, std::mutex> m_queue;
    std::mutex m_dataGuard;
    std::atomic<bool> m_sleepFlag;
    // TODO: this is crunches
    std::vector<std::pair<QObject*, bool>> m_connectedObjects;
};

} // namespace mpk::dss::backend::tamerlan
