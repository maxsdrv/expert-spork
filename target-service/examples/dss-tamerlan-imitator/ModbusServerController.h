#pragma once

#include "ModbusServer.h"

#include <QThread>
#include <QTimer>

#include <memory>

namespace mpk::dss::imitator::tamerlan
{

class ModbusServerController : public QObject
{
    Q_OBJECT

public:
    explicit ModbusServerController(
        std::unique_ptr<ModbusServer> server, QObject* parent = nullptr);

    Q_INVOKABLE void start();

    bool setData(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16 data);
    bool data(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16* data) const;

public slots:
    void stop();

private:
    void createAlarmTimers();
    void startAlarmTimers();

private slots:
    void toggleSensorAlarmStatus();

private:
    std::unique_ptr<ModbusServer> m_server;
    std::unique_ptr<QThread> m_serverThread;

    struct AlarmStatusRecord
    {
        int address{0};
        std::unique_ptr<QTimer> timer{nullptr};
    };

    std::map<int, std::vector<AlarmStatusRecord>> m_alarmRecords;
};

} // namespace mpk::dss::imitator::tamerlan
