#include "AuditEventLogger.h"
#include <string>

#include "log/Log2.h"

namespace mpk::dss::core
{

namespace
{
constexpr auto auditMessagePrefix = "Audit Event <";
constexpr auto deviceProviderCaption = "provider: ";
constexpr auto deviceClassCaption = "class: ";
constexpr auto deviceTypeCaption = "type: ";
constexpr auto deviceSerialCaption = "serial: ";
constexpr auto jammerBandCaption = "band: ";
constexpr auto alarmLevelCaption = "level: ";
constexpr auto clientAddrCaption = "client address: ";
} // namespace

// clang-format off
const std::unordered_map<AuditEventLogger::AuditEventType, std::string> AuditEventLogger::eventTypeStrs = {
    {AuditEventType::SuppressionOnManualRequested,  "SuppressionOnManualRequested"},
    {AuditEventType::SuppressionOffManualRequested, "SuppressionOffManualRequested"},
    {AuditEventType::SuppressionOnAutoRequested,    "SuppressionOnAutoRequested"},
    {AuditEventType::SuppressionOffAutoRequested,   "SuppressionOffAutoRequested"},
    {AuditEventType::SuppressionOnState,            "SuppressionOnState"},
    {AuditEventType::SuppressionOffState,           "SuppressionOffState"},
    {AuditEventType::Alarm,                         "Alarm"},
    {AuditEventType::DeviceStatusChanged,           "DeviceStatusChanged"},
    {AuditEventType::DeviceEnabled,                 "DeviceEnabled"},
    {AuditEventType::DeviceDisabled,                "DeviceDisabled"},
    // {AuditEventType::RestClientConnected,        "RestClientConnected"},
    // {AuditEventType::RestClientDisconnected,     "RestClientDisconnected"},
    {AuditEventType::WsClientConnected,             "WsClientConnected"},
    {AuditEventType::WsClientDisconnected,          "WsClientDisconnected"}};
// clang-format on

void AuditEventLogger::logSensorStateChanged(
    core::DeviceState::Value previousState,
    core::DeviceState::Value actualState,
    const QString& deviceClass,
    const QString& deviceType,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logSensorEvent(
        AuditEventType::DeviceStatusChanged,
        core::DeviceState::toString(previousState) + " -> "
            + core::DeviceState::toString(actualState),
        deviceClass,
        deviceType,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logJammerStateChanged(
    core::DeviceState::Value previousState,
    core::DeviceState::Value actualState,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logJammerEvent(
        AuditEventType::DeviceStatusChanged,
        core::DeviceState::toString(previousState) + " -> "
            + core::DeviceState::toString(actualState),
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logSensorDisabled(
    bool disabled,
    const QString& deviceClass,
    const QString& deviceType,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    auto eventDeviceDisabled = disabled ? AuditEventType::DeviceDisabled :
                                          AuditEventType::DeviceEnabled;
    logSensorEvent(
        eventDeviceDisabled,
        "",
        deviceClass,
        deviceType,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logJammerDisabled(
    bool disabled, const QString& deviceProvider, const QString& deviceSerial)
{
    auto eventDeviceDisabled = disabled ? AuditEventType::DeviceDisabled :
                                          AuditEventType::DeviceEnabled;
    logJammerEvent(eventDeviceDisabled, "", deviceProvider, deviceSerial);
}

void AuditEventLogger::logSuppressionManualRequested(
    const JammerBand& band,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logJammerEvent(
        band.active ? AuditEventType::SuppressionOnManualRequested :
                      AuditEventType::SuppressionOffManualRequested,
        jammerBandCaption + band.label,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logSuppressionAutoRequested(
    const JammerBand& band,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logJammerEvent(
        band.active ? AuditEventType::SuppressionOnManualRequested :
                      AuditEventType::SuppressionOffManualRequested,
        jammerBandCaption + band.label,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logSuppressionState(
    const JammerBand& band,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logJammerEvent(
        band.active ? AuditEventType::SuppressionOnState :
                      AuditEventType::SuppressionOffState,
        jammerBandCaption + band.label,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logAlarm(
    core::AlarmLevel::Value alarmLevel,
    const QString& deviceClass,
    const QString& deviceType,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logSensorEvent(
        AuditEventType::Alarm,
        alarmLevelCaption + core::AlarmLevel::toString(alarmLevel),
        deviceClass,
        deviceType,
        deviceProvider,
        deviceSerial);
}

void AuditEventLogger::logWsClientConnected(const QString& clientAddr)
{
    logEvent(
        AuditEventType::WsClientConnected, clientAddrCaption + clientAddr, "");
}

void AuditEventLogger::logWsClientDisconnected(const QString& clientAddr)
{
    logEvent(
        AuditEventType::WsClientDisconnected,
        clientAddrCaption + clientAddr,
        "");
}

void AuditEventLogger::logSensorEvent(
    AuditEventType eventType,
    const QString& eventDetails,
    const QString& deviceClass,
    const QString& deviceType,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logDeviceEvent(
        eventType,
        eventDetails,
        deviceProviderCaption + deviceProvider + ", " + deviceClassCaption
            + deviceClass + ", " + deviceTypeCaption + deviceType,
        deviceSerial);
}

void AuditEventLogger::logJammerEvent(
    AuditEventType eventType,
    const QString& eventDetails,
    const QString& deviceProvider,
    const QString& deviceSerial)
{
    logDeviceEvent(
        eventType,
        eventDetails,
        deviceProviderCaption + deviceProvider + ", " + deviceClassCaption
            + "jammer",
        deviceSerial);
}

void AuditEventLogger::logDeviceEvent(
    AuditEventType eventType,
    const QString& eventDetails,
    const QString& deviceInfo,
    const QString& deviceSerial)
{
    logEvent(
        eventType,
        eventDetails,
        deviceInfo + ", " + deviceSerialCaption + deviceSerial);
}

void AuditEventLogger::logEvent(
    AuditEventType eventType,
    const QString& eventDetails,
    const QString& deviceInfo)
{
    auto event = eventTypeStrs.find(eventType);
    if (event == eventTypeStrs.end())
    {
        LOG_WARNING << "Invalid audit event: " << static_cast<int>(eventType);
        return;
    }

    std::string source;
    if (!deviceInfo.isEmpty())
    {
        source = "Device " + deviceInfo.toStdString();
    }
    LOG_INFO << auditMessagePrefix << event->second << "> "
             << eventDetails.toStdString() << " [" << source << "]";
}

} // namespace mpk::dss::core
