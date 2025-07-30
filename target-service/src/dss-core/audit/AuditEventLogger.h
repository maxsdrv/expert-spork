#pragma once

#include "dss-core/AlarmLevel.h"
#include "dss-core/DeviceState.h"
#include "dss-core/JammerBand.h"

#include <QString>

#include <unordered_map>

namespace mpk::dss::core
{

class AuditEventLogger final
{
public:
    AuditEventLogger(const AuditEventLogger&) = delete;
    AuditEventLogger(AuditEventLogger&&) = delete;
    AuditEventLogger& operator=(const AuditEventLogger&) = delete;
    AuditEventLogger& operator=(AuditEventLogger&&) = delete;

    static void logSensorStateChanged(
        core::DeviceState::Value previousState,
        core::DeviceState::Value actualState,
        const QString& deviceClass,
        const QString& deviceType,
        const QString& deviceProvider,
        const QString& deviceSerial);
    static void logJammerStateChanged(
        core::DeviceState::Value previousState,
        core::DeviceState::Value actualState,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logSensorDisabled(
        bool disabled,
        const QString& deviceClass,
        const QString& deviceType,
        const QString& deviceProvider,
        const QString& deviceSerial);
    static void logJammerDisabled(
        bool disabled,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logSuppressionManualRequested(
        const JammerBand& band,
        const QString& deviceProvider,
        const QString& deviceSerial);
    static void logSuppressionAutoRequested(
        const JammerBand& band,
        const QString& deviceProvider,
        const QString& deviceSerial);
    static void logSuppressionState(
        const JammerBand& band,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logAlarm(
        core::AlarmLevel::Value,
        const QString& deviceClass,
        const QString& deviceType,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logWsClientConnected(const QString& clientAddr);
    static void logWsClientDisconnected(const QString& clientAddr);

private:
    AuditEventLogger() = default;
    ~AuditEventLogger() = default;

    static AuditEventLogger& instance()
    {
        static AuditEventLogger instance;
        return instance;
    }
    friend inline AuditEventLogger& auditLogger();

    enum class AuditEventType
    {
        SuppressionOnManualRequested = 0,
        SuppressionOffManualRequested,
        SuppressionOnAutoRequested,
        SuppressionOffAutoRequested,
        SuppressionOnState,
        SuppressionOffState,
        Alarm = 25,
        DeviceStatusChanged = 30,
        DeviceEnabled,
        DeviceDisabled,
        WsClientConnected = 40,
        WsClientDisconnected
        // RestClientConnected,
        // RestClientDisconnected,
    };
    static const std::unordered_map<AuditEventLogger::AuditEventType, std::string>
        eventTypeStrs;

    static void logSensorEvent(
        AuditEventType eventType,
        const QString& eventDetails,
        const QString& deviceClass,
        const QString& deviceType,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logJammerEvent(
        AuditEventType eventType,
        const QString& eventDetails,
        const QString& deviceProvider,
        const QString& deviceSerial);

    static void logDeviceEvent(
        AuditEventType eventType,
        const QString& eventDetails,
        const QString& deviceInfo,
        const QString& deviceSerial);

    static void logEvent(
        AuditEventType eventType,
        const QString& eventDetails,
        const QString& deviceInfo);
};

inline AuditEventLogger& auditLogger()
{
    return AuditEventLogger::instance();
}

} // namespace mpk::dss::core
