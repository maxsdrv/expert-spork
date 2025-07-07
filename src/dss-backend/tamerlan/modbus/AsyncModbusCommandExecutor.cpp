#include "AsyncModbusCommandExecutor.h"

#include "dss-backend/tamerlan/modbus/ModbusConnection.h"

#include "log/Log2.h"

#include "gsl/pointers"

namespace mpk::dss::backend::tamerlan
{

constexpr auto maxQueueSize = 1000U;

AsyncModbusCommandExecutor::AsyncModbusCommandExecutor(
    std::unique_ptr<ModbusConnection> connection, QObject* parent) :
  ModbusCommandExecutor{std::move(connection), parent}, m_sleepFlag{true}
{
    connect(
        this,
        &AsyncModbusCommandExecutor::wokenUp,
        this,
        &AsyncModbusCommandExecutor::process,
        Qt::QueuedConnection);
}

AsyncModbusCommandExecutor::~AsyncModbusCommandExecutor() = default;

void AsyncModbusCommandExecutor::readInputRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint16_t* destination,
    ModbusServerAddress serverAddress)
{
    auto safeModbusCall = [this, address, numRegs, destination, serverAddress]()
    {
        std::vector<uint16_t> buffer(numRegs);
        connection()->readInputRegisters(
            address, numRegs, buffer.data(), serverAddress);
        m_dataGuard.lock();
        std::copy(buffer.begin(), buffer.end(), destination);
        m_dataGuard.unlock();
    };

    pushCall(IdCallPair(commandId, safeModbusCall));
    setSleepFlag(false);
}

void AsyncModbusCommandExecutor::readHoldingRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint16_t* destination,
    ModbusServerAddress serverAddress)
{
    auto safeModbusCall = [this, address, numRegs, destination, serverAddress]()
    {
        std::vector<uint16_t> buffer(numRegs);
        connection()->readHoldingRegisters(
            address, numRegs, buffer.data(), serverAddress);
        m_dataGuard.lock();
        std::copy(buffer.begin(), buffer.end(), destination);
        m_dataGuard.unlock();
    };

    pushCall(IdCallPair(commandId, safeModbusCall));
    setSleepFlag(false);
}

void AsyncModbusCommandExecutor::readCoils(
    const CommandId& commandId,
    int address,
    int numRegs,
    uint8_t* destination,
    ModbusServerAddress serverAddress)
{
    auto safeModbusCall = [this, address, numRegs, destination, serverAddress]()
    {
        std::vector<uint8_t> buffer(numRegs);
        connection()->readCoils(address, numRegs, buffer.data(), serverAddress);
        m_dataGuard.lock();
        std::copy(buffer.begin(), buffer.end(), destination);
        m_dataGuard.unlock();
    };

    pushCall(IdCallPair(commandId, safeModbusCall));
    setSleepFlag(false);
}

void AsyncModbusCommandExecutor::writeHoldingRegisters(
    const CommandId& commandId,
    int address,
    int numRegs,
    const uint16_t* source,
    ModbusServerAddress serverAddress)
{
    auto safeModbusCall = [this, address, numRegs, source, serverAddress]()
    {
        m_dataGuard.lock();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::vector<uint16_t> buffer{source, source + numRegs};
        m_dataGuard.unlock();
        connection()->writeHoldingRegisters(
            address, numRegs, buffer.data(), serverAddress);
    };

    pushCall(IdCallPair(commandId, safeModbusCall));
    setSleepFlag(false);
}

void AsyncModbusCommandExecutor::writeCoils(
    const CommandId& commandId,
    int address,
    int numRegs,
    const uint8_t* source,
    ModbusServerAddress serverAddress)
{
    auto safeModbusCall = [this, address, numRegs, source, serverAddress]()
    {
        m_dataGuard.lock();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        std::vector<uint8_t> buffer{source, source + numRegs};
        m_dataGuard.unlock();

        LOG_TRACE << numRegs << " bytes to write, buffer size "
                  << buffer.size();
        connection()->writeCoils(address, numRegs, buffer.data(), serverAddress);
    };

    pushCall(IdCallPair(commandId, safeModbusCall));
    setSleepFlag(false);
}

void AsyncModbusCommandExecutor::setDisabled(bool disabled)
{
    ModbusCommandExecutor::setDisabled(disabled);
    if (disabled)
    {
        m_queue.clear();
    }
}

void AsyncModbusCommandExecutor::lock()
{
    m_dataGuard.lock();
}

void AsyncModbusCommandExecutor::unlock()
{
    m_dataGuard.unlock();
}

void AsyncModbusCommandExecutor::addObject(QObject* object)
{
    m_connectedObjects.emplace_back(object, true);
}

void AsyncModbusCommandExecutor::setDisabled(QObject* object, bool disabled)
{
    auto it = std::find_if(
        m_connectedObjects.begin(),
        m_connectedObjects.end(),
        [object](const auto& data) { return data.first == object; });
    Ensures(it != m_connectedObjects.end());
    it->second = !disabled;

    auto disableConnection = std::all_of(
        m_connectedObjects.begin(),
        m_connectedObjects.end(),
        [](const auto& data) { return data.second == false; });
    setDisabled(disableConnection);
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void AsyncModbusCommandExecutor::process()
{
    if (auto idCallPair = m_queue.tryPop())
    {
        try
        {
            (idCallPair->second)();
            emit succeeded(idCallPair->first);
        }
        catch (const std::exception& ex)
        {
            auto errorStr =
                QString("Error while running command: %1").arg(ex.what());
            LOG_ERROR << errorStr.toStdString();
            emit failed(idCallPair->first, errorStr);
        }
    }

    setSleepFlag(m_queue.isEmpty());
}

void AsyncModbusCommandExecutor::setSleepFlag(bool sleepFlag)
{
    if (!sleepFlag)
    {
        emit wokenUp();
    }

    bool oldSleepFlag = m_sleepFlag.exchange(sleepFlag);
    if (!oldSleepFlag && sleepFlag)
    {
        emit fellAsleep();
    }
}

void AsyncModbusCommandExecutor::pushCall(const IdCallPair& call)
{
    while (m_queue.size() >= maxQueueSize)
    {
        m_queue.pop();
    }
    m_queue.push(call);
}

} // namespace mpk::dss::backend::tamerlan
