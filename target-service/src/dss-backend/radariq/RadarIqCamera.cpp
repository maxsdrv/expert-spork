#include "RadarIqCamera.h"

#include "RadarIqController.h"

#include "dss-core/audit/AuditEventLogger.h"

#include "log/Log2.h"

namespace mpk::dss::backend::radariq
{
constexpr double circleAngle = 360.0;
constexpr auto imitatorSuffix = "Imitator";

using namespace std::chrono_literals;

RadarIqCamera::RadarIqCamera(
    std::shared_ptr<RadarIqConnection> connection,
    QString model,
    int distance,
    QObject* parent) :
  QObject{parent},
  m_connection(std::move(connection)),
  m_model(std::move(model)),
  m_positionSignals{std::make_unique<core::DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<core::DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<core::DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<core::DeviceStateSignals>()},
  m_hwInfoSignals{std::make_unique<core::DeviceHwInfoSignals>()}
{
    m_workzone.sectors.emplace_back(core::DeviceWorkzoneSector{
        0, static_cast<double>(distance), 0, circleAngle});

    connect(
        m_connection.get(),
        &RadarIqConnection::updateStarted,
        this,
        &RadarIqCamera::updateState);
    if (m_model.isEmpty())
    {
        connect(
            m_connection.get(),
            &RadarIqConnection::cameraInfoUpdated,
            this,
            &RadarIqCamera::updateModel);
    }
    connect(
        m_connection.get(),
        &RadarIqConnection::generalSettingsUpdated,
        this,
        &RadarIqCamera::updateGeneralSettings);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarPowerStatusReceived,
        this,
        &RadarIqCamera::updateRadarPowerStatus);
    connect(
        m_connection.get(),
        &RadarIqConnection::cameraPositionUpdated,
        this,
        &RadarIqCamera::updateCameraPosition);
    connect(
        m_connection.get(),
        &RadarIqConnection::cameraConnectionStatusReceived,
        this,
        &RadarIqCamera::updateConnectionStatus);
}

RadarIqCamera::~RadarIqCamera() = default;

core::DeviceType::Value RadarIqCamera::type() const
{
    return core::DeviceType::CAMERA;
}

RadarIqCamera::Serial RadarIqCamera::serial() const
{
    return m_serial;
}

RadarIqCamera::Model RadarIqCamera::model() const
{
    if (m_isImitator)
    {
        return m_model + " " + imitatorSuffix;
    }
    return m_model;
}

RadarIqCamera::Version RadarIqCamera::swVersion() const
{
    return m_swVersion;
}

auto RadarIqCamera::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-camera-" + m_serial;
    }
    return {};
}

bool RadarIqCamera::disabled() const
{
    return m_disabled;
}

void RadarIqCamera::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        m_disabled = disabled;

        core::auditLogger().logSensorDisabled(
            disabled,
            core::DeviceClass::toString(deviceClass),
            core::DeviceType::toString(type()),
            nodeKey,
            m_serial);
        emit m_disabledSignals->disabledChanged(disabled);
    }
}

gsl::not_null<const core::DeviceDisabledSignals*> RadarIqCamera::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

core::DevicePosition RadarIqCamera::position() const
{
    return m_positionMode == core::DevicePositionMode::AUTO ? m_autoPosition :
                                                              m_manualPosition;
}

void RadarIqCamera::setPosition(const core::DevicePosition& position)
{
    if (m_manualPosition != position)
    {
        m_manualPosition = position;
        if (m_positionMode == core::DevicePositionMode::MANUAL
            || m_positionMode == core::DevicePositionMode::ALWAYS_MANUAL)
        {
            emit m_positionSignals->positionChanged();
        }
    }
}

core::DevicePositionMode RadarIqCamera::positionMode() const
{
    return m_positionMode;
}

void RadarIqCamera::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> RadarIqCamera::positionSignals()
    const
{
    return m_positionSignals.get();
}

core::DeviceWorkzone RadarIqCamera::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> RadarIqCamera::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value RadarIqCamera::state() const
{
    return m_state;
}

gsl::not_null<const core::DeviceStateSignals*> RadarIqCamera::stateSignals()
    const
{
    return m_stateSignals.get();
}

core::DeviceHwInfo RadarIqCamera::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> RadarIqCamera::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

void RadarIqCamera::setImitatorMark(bool isImitator)
{
    m_isImitator = isImitator;
}

void RadarIqCamera::updateState(bool connected, bool faultless)
{
    auto state = core::DeviceState::UNAVAILABLE;

    if (connected)
    {
        state = core::DeviceState::OFF;

        if (m_radarPowerOn)
        {
            if (m_cameraConnectionOk && faultless)
            {
                state = core::DeviceState::OK;
            }
            else
            {
                state = core::DeviceState::FAILURE;
            }
        }
    }

    if (state != m_state)
    {
        core::auditLogger().logSensorStateChanged(
            m_state,
            state,
            core::DeviceClass::toString(deviceClass),
            core::DeviceType::toString(type()),
            nodeKey,
            m_serial);

        m_state = state;
        emit m_stateSignals->stateChanged(m_state);
    }
}

void RadarIqCamera::updateModel(const QString& model)
{
    m_model = model;
}

void RadarIqCamera::updateGeneralSettings(
    const OARadarIqDevice_general_settings& settings)
{
    m_serial = settings.getId();
}

void RadarIqCamera::updateRadarPowerStatus(const bool isPowerOn)
{
    m_radarPowerOn = isPowerOn;
}

void RadarIqCamera::updateCameraPosition(const OARadarIqGeo_position& position)
{
    auto updatedPosition = core::DevicePosition{
        .azimuth = position.getAzimuth(),
        .coordinate = QGeoCoordinate{
            position.getCoordinate().getLatitude(),
            position.getCoordinate().getLongitude(),
            position.getCoordinate().getAltitude()}};

    auto oldAutoPosition = m_autoPosition;
    m_autoPosition = updatedPosition;
    if (m_positionMode == core::DevicePositionMode::AUTO
        && updatedPosition != oldAutoPosition)
    {
        emit m_positionSignals->positionChanged();
    }
}

void RadarIqCamera::updateConnectionStatus(const bool isConnection)
{
    m_cameraConnectionOk = isConnection;
}

} // namespace mpk::dss::backend::radariq
