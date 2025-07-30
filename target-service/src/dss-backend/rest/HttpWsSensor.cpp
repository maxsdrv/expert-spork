#include "HttpWsSensor.h"

#include "RestController.h"

#include "converters/DevicePositionVsGeolocation.h"
#include "converters/JammerIdsVsChannelInfo.h"
#include "converters/SensorWorkzoneVsChannelInfo.h"
#include "dss-common/api/Helpers.h"
#include "geolocation/Geolocation.h"
#include "json/ChannelInfoVsJson.h"
#include "json/DiagnosticDataVsJson.h"
#include "json/GeolocationVsJson.h"

#include "log/Log2.h"
#include "mpk/rest/HttpWsClient.h"
#include "mpk/rest/HttpWsConnection.h"

#include "httpws/ws/WebSocketClient.h"

namespace mpk::dss::backend::rest
{

using namespace std::chrono_literals;
using namespace mpk::dss::core;

HttpWsSensor::HttpWsSensor(
    std::unique_ptr<mpk::rest::HttpWsConnection> connection,
    const std::chrono::milliseconds& updateIntervalSettings,
    const std::chrono::milliseconds& /*timeoutSettings*/,
    QObject* parent) :
  QObject{parent},
  m_connection{std::move(connection)},
  m_client{std::make_unique<mpk::rest::HttpWsClient>(m_connection.get())},
  m_updateInterval{updateIntervalSettings},
  m_serial{QUuid::createUuid().toString(QUuid::WithoutBraces)},
  m_model{"Generic Imitator"},
  m_swVersion{"0.0.1"},
  m_positionSignals{std::make_unique<DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<DeviceStateSignals>()},
  m_jammerSignals{std::make_unique<SensorJammerSignals>()},
  m_hwInfoSignals{std::make_unique<DeviceHwInfoSignals>()}
{
    connectSignalsToSlots();
    m_connection->start();
    m_updateTimer.start(m_updateInterval);
}

HttpWsSensor::~HttpWsSensor() = default;

void HttpWsSensor::connectSignalsToSlots()
{
    connect(&m_updateTimer, &QTimer::timeout, this, &HttpWsSensor::updateData);

    connect(
        this,
        &HttpWsSensor::updateTimerDisabled,
        this,
        &HttpWsSensor::restartTimer);

    QObject::connect(
        this,
        &HttpWsSensor::toggleWsConnection,
        this,
        [this](bool enabled)
        { enabled ? m_connection->start() : m_connection->stop(); });
}

bool HttpWsSensor::isConnectionOk()
{
    return !m_updateTimeout
           && m_connection->state() == mpk::rest::ConnectionState::CONNECTED;
}

bool HttpWsSensor::isDeviceConditionOk() const
{
    return m_deviceConditionOk && !m_httpError;
}

DeviceType::Value HttpWsSensor::type() const
{
    return DeviceType::RFD;
}

auto HttpWsSensor::serial() const -> Serial
{
    return m_serial;
}

auto HttpWsSensor::model() const -> Model
{
    return m_model;
}

auto HttpWsSensor::swVersion() const -> Version
{
    return m_swVersion;
}

auto HttpWsSensor::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_serial;
    }
    return {};
}

bool HttpWsSensor::disabled() const
{
    return m_disabled;
}

void HttpWsSensor::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        m_disabled = disabled;
        emit m_disabledSignals->disabledChanged(disabled);

        emit updateTimerDisabled();
        emit toggleWsConnection(!m_disabled);
    }
}

gsl::not_null<const core::DeviceDisabledSignals*> HttpWsSensor::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

core::DevicePosition HttpWsSensor::position() const
{
    return m_positionMode == DevicePositionMode::AUTO ? m_autoPosition :
                                                        m_manualPosition;
}

void HttpWsSensor::setPosition(const core::DevicePosition& position)
{
    if (m_manualPosition != position)
    {
        m_manualPosition = position;
        if (m_positionMode == DevicePositionMode::MANUAL
            || m_positionMode == DevicePositionMode::ALWAYS_MANUAL)
        {
            emit m_positionSignals->positionChanged();
        }
    }
}

DevicePositionMode HttpWsSensor::positionMode() const
{
    return m_positionMode;
}

void HttpWsSensor::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> HttpWsSensor::positionSignals()
    const
{
    return m_positionSignals.get();
}

core::DeviceWorkzone HttpWsSensor::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> HttpWsSensor::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value HttpWsSensor::state() const
{
    return m_state;
}

gsl::not_null<const core::DeviceStateSignals*> HttpWsSensor::stateSignals() const
{
    return m_stateSignals.get();
}

core::DeviceHwInfo HttpWsSensor::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> HttpWsSensor::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

void HttpWsSensor::restartTimer()
{
    m_disabled ? m_updateTimer.stop() : m_updateTimer.start();
}

void HttpWsSensor::updateData()
{
    LOG_TRACE << "timer";
    m_updateTimeout = false;
    m_httpError = false;
    requestChannelInfo();
    requestPosition();
    requestDiagnosticData();
    updateState();
}

gsl::not_null<mpk::rest::HttpWsConnection*> HttpWsSensor::connection() const
{
    return m_connection.get();
}

void HttpWsSensor::requestChannelInfo()
{
    LOG_TRACE << "request channel_info";
    m_client->get(
        "get_channel_info",
        {},
        [this](const QJsonObject& object)
        {
            LOG_TRACE << object;
            auto channelInfo = json::fromValue<ChannelInfo>(object);
            updateWorkzone(channelInfo);
            updateJammerIds(channelInfo);
        },
        [this](int errorCode)
        {
            api::handleRequestFailure(
                "channel info", m_updateTimeout, m_httpError, errorCode);
        });
}

void HttpWsSensor::requestDiagnosticData()
{
    LOG_TRACE << "request diagnostic data";
    m_client->get(
        "get_diagnostic_data",
        {},
        [this](const QJsonObject& object)
        {
            auto diagnostic = json::fromValue<DeviceDiagnosticData>(object);
            LOG_TRACE << "received diagnostic data: " << diagnostic.state << " "
                      << diagnostic.cpuTemp;
            m_deviceConditionOk =
                diagnostic.state == DeviceDiagnosticData::Tags::ok;
            // TODO
            const auto voltage = 48.0;
            const auto amperage = 10.0;
            updateHwInfo(diagnostic.cpuTemp, voltage, amperage);
        },
        [this](int errorCode)
        {
            api::handleRequestFailure(
                "diagnostic data", m_updateTimeout, m_httpError, errorCode);
        });
}

void HttpWsSensor::requestPosition()
{
    LOG_TRACE << "request position";
    m_client->get(
        "get_geolocation",
        {},
        [this](const QJsonObject& object)
        {
            auto geolocation = json::fromValue<Geolocation>(object);
            auto position = DevicePositionVsGeolocation::convert(geolocation);
            LOG_TRACE << "received geolocation for position update: "
                      << position.coordinate;
            LOG_TRACE << "received azimuth for position update: "
                      << position.azimuth;

            auto oldAutoPosition = m_autoPosition;
            m_autoPosition = position;
            if (m_positionMode == core::DevicePositionMode::AUTO
                && position != oldAutoPosition)
            {
                emit m_positionSignals->positionChanged();
            }
        },
        [this](int errorCode)
        {
            api::handleRequestFailure(
                "position", m_updateTimeout, m_httpError, errorCode);
        });
}

void HttpWsSensor::requestWorkzone()
{
    LOG_TRACE << "request workzone";
    m_client->get(
        "get_channel_info",
        {},
        [this](const QJsonObject& object)
        {
            auto channelInfo = json::fromValue<ChannelInfo>(object);
            auto workzone = DeviceWorkzoneVsChannelInfo::convert(channelInfo);

            if (workzone != m_workzone)
            {
                m_workzone = workzone;
                emit m_workzoneSignals->workzoneChanged(m_workzone);
            }
        },
        [this](int errorCode)
        {
            api::handleRequestFailure(
                "workzone", m_updateTimeout, m_httpError, errorCode);
        });
}

void HttpWsSensor::updateJammerIds(const ChannelInfo& channelInfo)
{
    m_jammerIds = JammerIdsVsChannelInfo::convert(channelInfo);
}

void HttpWsSensor::updateHwInfo(double temp, double voltage, double current)
{
    auto hwInfo = core::DeviceHwInfo{
        {{core::DeviceHwInfo::Tags::temperature,
          QString::number(temp).append(" ℃")},
         {core::DeviceHwInfo::Tags::voltage,
          QString::number(voltage).append(" V")},
         {core::DeviceHwInfo::Tags::current,
          QString::number(current).append(" mA")}}};

    if (hwInfo != m_hwInfo)
    {
        m_hwInfo = hwInfo;
        emit m_hwInfoSignals->hwInfoChanged(m_hwInfo);
    }
}

void HttpWsSensor::updateState()
{
    auto state = core::DeviceState::UNAVAILABLE;

    if (isConnectionOk())
    {
        if (m_deviceConditionOk)
        {
            state = core::DeviceState::OK;
        }
        else
        {
            state = core::DeviceState::FAILURE;
        }
    }

    if (state != m_state)
    {
        m_state = state;
        emit m_stateSignals->stateChanged(m_state);
    }
}

void HttpWsSensor::updateWorkzone(const ChannelInfo& channelInfo)
{
    auto workzone = DeviceWorkzoneVsChannelInfo::convert(channelInfo);
    if (workzone != m_workzone)
    {
        m_workzone = workzone;
        emit m_workzoneSignals->workzoneChanged(m_workzone);
    }
}

} // namespace mpk::dss::backend::rest
