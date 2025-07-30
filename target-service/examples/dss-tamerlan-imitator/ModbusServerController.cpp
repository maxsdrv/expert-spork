#include "ModbusServerController.h"

#include "SensorChannelParameters.h"

#include "dss-backend/tamerlan/modbus/Protocol.h"
#include "dss-backend/tamerlan/modbus/TransmissionData.h"

#include "log/Log2.h"

#include "exception/General.h"

#include <QDateTime>
#include <QThread>

#include <chrono>
#include <random>

namespace mpk::dss::imitator::tamerlan
{

namespace
{

using namespace std::chrono_literals;
constexpr auto alarmMinInterval = 15s;
constexpr auto alarmMaxInterval = 30s;

std::default_random_engine& randomEngine()
{
    static std::default_random_engine u{};
    return u;
}

void randomize()
{
    static std::random_device rd{};
    randomEngine().seed(rd());
}

std::chrono::seconds alarmInterval()
{
    static std::uniform_int_distribution<> d{};
    using param_t = decltype(d)::param_type;
    auto interval =
        d(randomEngine(),
          param_t{alarmMinInterval.count(), alarmMaxInterval.count()});
    return static_cast<std::chrono::seconds>(interval);
}

} // namespace

ModbusServerController::ModbusServerController(
    std::unique_ptr<ModbusServer> server, QObject* parent) :
  QObject(parent), m_server{std::move(server)}
{
    randomize();
}

void ModbusServerController::start()
{
    if (m_server)
    {
        m_serverThread = std::make_unique<QThread>();
        m_server->moveToThread(m_serverThread.get());
        m_serverThread->start();

        QMetaObject::invokeMethod(m_server.get(), "start", Qt::QueuedConnection);

        createAlarmTimers();
        startAlarmTimers();
    }
}

void ModbusServerController::stop()
{
    if (m_server)
    {
        m_server->stop();
    }
}

void ModbusServerController::createAlarmTimers()
{
    using namespace mpk::dss::backend::tamerlan::proto;

    auto serverAddresses = m_server->serverAddresses();
    for (const auto serverAddress: serverAddresses)
    {
        if (m_server->serviceRole(serverAddress) == ServiceRole::SensorImitator)
        {
            quint16 numChannels{0};
            auto ok = data(
                serverAddress,
                QModbusDataUnit::InputRegisters,
                ChannelsCount::address,
                &numChannels);
            if (ok)
            {
                m_alarmRecords[serverAddress].resize(numChannels);
                for (auto i = 0; i != numChannels; i++)
                {
                    quint16 sensorStatus{0};
                    quint16 address = firstChannelDataAddress
                                      + i * nextSensorChannelDataOffset;
                    data(
                        serverAddress,
                        QModbusDataUnit::InputRegisters,
                        address,
                        &sensorStatus);

                    if (static_cast<SensorStatus>(sensorStatus)
                        == SensorStatus::Alarm)
                    {
                        m_alarmRecords[serverAddress].at(i) = {
                            .address = address,
                            .timer = std::make_unique<QTimer>()};
                        auto* timer =
                            m_alarmRecords[serverAddress].at(i).timer.get();
                        connect(
                            timer,
                            &QTimer::timeout,
                            this,
                            &ModbusServerController::toggleSensorAlarmStatus,
                            Qt::DirectConnection);
                        timer->setInterval(alarmInterval());
                    }
                }
            }
        }
    }
}

void ModbusServerController::startAlarmTimers()
{
    for (auto& rec: m_alarmRecords)
    {
        for (auto& record: rec.second)
        {
            if (record.timer)
            {
                record.timer->start();
            }
        }
    }
}

void ModbusServerController::toggleSensorAlarmStatus()
{
    using namespace mpk::dss::backend::tamerlan::proto;
    static unsigned int alarmId = 0;

    auto* sender = QObject::sender();
    for (auto& rec: m_alarmRecords)
    {
        auto record = std::find_if(
            rec.second.begin(),
            rec.second.end(),
            [&sender](const AlarmStatusRecord& record)
            { return record.timer.get() == sender; });

        if (record != rec.second.end())
        {
            quint16 value{0};
            auto ok = data(
                rec.first,
                QModbusDataUnit::InputRegisters,
                record->address,
                &value);
            if (ok)
            {
                auto status = static_cast<SensorStatus>(value);
                auto newStatus = (status == SensorStatus::Alarm) ?
                                     SensorStatus::NoAlarm :
                                     SensorStatus::Alarm;
                setData(
                    rec.first,
                    QModbusDataUnit::InputRegisters,
                    record->address + ChannelStatus::relativeAddress,
                    static_cast<quint16>(newStatus));

                setData(
                    rec.first,
                    QModbusDataUnit::InputRegisters,
                    record->address + ChannelObjectsCount::relativeAddress,
                    newStatus == SensorStatus::Alarm ? 1 : 0);

                // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                backend::tamerlan::TransmissionData alarmData{
                    .id = alarmId++,
                    .handbookId = 1,
                    .timestamp =
                        QDateTime::currentDateTime().toMSecsSinceEpoch(),
                    .detectionLevel = 2,
                    .detectionThreshold = 6,
                    .beginFreq = 500,
                    .endFreq = 550,
                    .detectionProbability = 0.75F};
                // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                alarmData.name[0] = 'F';
                alarmData.name[1] = 'P';
                alarmData.name[2] = 'V';

                auto regs = std::vector<std::uint16_t>{};
                backend::tamerlan::TransmissionData::toRegs(
                    alarmData, std::back_inserter(regs));
                for (int i = 0; i < static_cast<int>(regs.size()); i++)
                {
                    setData(
                        rec.first,
                        QModbusDataUnit::InputRegisters,
                        record->address + Alarm::relativeAddress + i,
                        regs.at(i));
                }

                record->timer->setInterval(alarmInterval());
            }
            break;
        }
    }
}

bool ModbusServerController::setData(
    int serverAddress,
    QModbusDataUnit::RegisterType table,
    quint16 address,
    quint16 data)
{
    return m_server->setData(serverAddress, table, address, data);
}

bool ModbusServerController::data(
    int serverAddress,
    QModbusDataUnit::RegisterType table,
    quint16 address,
    quint16* data) const
{
    return m_server->data(serverAddress, table, address, data);
}

} // namespace mpk::dss::imitator::tamerlan
