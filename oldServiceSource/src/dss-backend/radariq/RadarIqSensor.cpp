#include "RadarIqSensor.h"

#include "RadarIqController.h"

#include "dss-core/audit/AuditEventLogger.h"

#include "log/Log2.h"

namespace mpk::dss::backend::radariq
{
constexpr double circleAngle = 360.0;
constexpr auto imitatorSuffix = "Imitator";

using namespace std::chrono_literals;

RadarIqSensor::RadarIqSensor(
    std::shared_ptr<RadarIqConnection> connection,
    QString model,
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
    connect(
        m_connection.get(),
        &RadarIqConnection::updateStarted,
        this,
        &RadarIqSensor::updateState);
    if (m_model.isEmpty())
    {
        connect(
            m_connection.get(),
            &RadarIqConnection::radarInfoUpdated,
            this,
            &RadarIqSensor::updateModel);
    }
    connect(
        m_connection.get(),
        &RadarIqConnection::generalSettingsUpdated,
        this,
        &RadarIqSensor::updateGeneralSettings);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarPositionUpdated,
        this,
        &RadarIqSensor::updateRadarPosition);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarWorkzoneUpdated,
        this,
        &RadarIqSensor::updateRadarWorkzone);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarTemperatureUpdated,
        this,
        &RadarIqSensor::updateRadarTemperature);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarModeUpdated,
        this,
        &RadarIqSensor::updateRadarMode);
    connect(
        m_connection.get(),
        &RadarIqConnection::radarPowerStatusReceived,
        this,
        &RadarIqSensor::updateRadarPowerStatus);
}

RadarIqSensor::~RadarIqSensor() = default;

core::DeviceType::Value RadarIqSensor::type() const
{
    return core::DeviceType::RADAR;
}

RadarIqSensor::Serial RadarIqSensor::serial() const
{
    return m_serial;
}

RadarIqSensor::Model RadarIqSensor::model() const
{
    if (m_isImitator)
    {
        return m_model + " " + imitatorSuffix;
    }
    return m_model;
}

RadarIqSensor::Version RadarIqSensor::swVersion() const
{
    return m_swVersion;
}

auto RadarIqSensor::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_serial;
    }
    return {};
}

bool RadarIqSensor::disabled() const
{
    return m_disabled;
}

void RadarIqSensor::setDisabled(bool disabled)
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

gsl::not_null<const core::DeviceDisabledSignals*> RadarIqSensor::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

core::DevicePosition RadarIqSensor::position() const
{
    return m_positionMode == core::DevicePositionMode::AUTO ? m_autoPosition :
                                                              m_manualPosition;
}

void RadarIqSensor::setPosition(const core::DevicePosition& position)
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

core::DevicePositionMode RadarIqSensor::positionMode() const
{
    return m_positionMode;
}

void RadarIqSensor::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> RadarIqSensor::positionSignals()
    const
{
    return m_positionSignals.get();
}

core::DeviceWorkzone RadarIqSensor::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> RadarIqSensor::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value RadarIqSensor::state() const
{
    return m_state;
}

gsl::not_null<const core::DeviceStateSignals*> RadarIqSensor::stateSignals()
    const
{
    return m_stateSignals.get();
}

core::DeviceHwInfo RadarIqSensor::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> RadarIqSensor::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

core::DevicePosition RadarIqSensor::autoPosition() const
{
    return m_autoPosition;
}

core::DevicePosition RadarIqSensor::manualPosition() const
{
    return m_manualPosition;
}

void RadarIqSensor::updateState(bool connected, bool faultless)
{
    auto state = core::DeviceState::UNAVAILABLE;

    if (connected)
    {
        state = core::DeviceState::OFF;

        if (m_radarPowerOn)
        {
            if (m_radarConditionOk && faultless)
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

void RadarIqSensor::updateModel(const QString& model)
{
    m_model = model;
}

void RadarIqSensor::updateGeneralSettings(
    const OARadarIqDevice_general_settings& settings)
{
    m_serial = settings.getId();
}

void RadarIqSensor::updateRadarPosition(const OARadarIqGeo_position& position)
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

void RadarIqSensor::updateRadarWorkzone(const QList<OARadarIqSector>& sectors)
{
    auto workzone = core::DeviceWorkzone{};
    auto sectorWidthAngle = circleAngle / sectors.count();
    int sectorNumber = 0;

    for (const auto& sector: sectors)
    {
        double minAngle = 0;
        double maxAngle = 0;
        double distance = 0;

        const auto zones = sector.getZones();
        for (const auto& zone: zones)
        {
            if (!zone.isEnabled())
            {
                continue;
            }
            minAngle = std::min(minAngle, zone.getAzimuthMin());
            maxAngle = std::max(maxAngle, zone.getAzimuthMax());
            distance = std::max(distance, zone.getRadiusMax());
        }

        if (distance != 0)
        {
            minAngle += sectorNumber * sectorWidthAngle;
            maxAngle += sectorNumber * sectorWidthAngle;
            workzone.sectors.emplace_back(core::DeviceWorkzoneSector{
                sectorNumber, distance, minAngle, maxAngle});
        }
        sectorNumber++;
    }

    if (workzone != m_workzone)
    {
        m_workzone = workzone;
        emit m_workzoneSignals->workzoneChanged(m_workzone);
    }
}

void RadarIqSensor::updateRadarTemperature(const double temperature)
{
    LOG_TRACE << "temperature: " << temperature;

    auto updatedTemperature = QString::number(temperature, 'f', 1) + " ℃";

    if (m_hwInfo.values[core::DeviceHwInfo::Tags::temperature]
        != updatedTemperature)
    {
        m_hwInfo.values[core::DeviceHwInfo::Tags::temperature] =
            updatedTemperature;
        emit m_hwInfoSignals->hwInfoChanged(m_hwInfo);
    }
}

void RadarIqSensor::updateRadarMode(const OARadarIqRadar_mode& mode)
{
    LOG_TRACE << "mode: " << mode.asJson();

    switch (mode.getValue())
    {
        case OARadarIqRadar_mode::eOARadarIqRadar_mode::PROBE:
        case OARadarIqRadar_mode::eOARadarIqRadar_mode::SCAN:
        case OARadarIqRadar_mode::eOARadarIqRadar_mode::IMITATION:
            m_radarConditionOk = true;
            break;
        default: m_radarConditionOk = false;
    }
}

void RadarIqSensor::updateRadarPowerStatus(const bool isPowerOn)
{
    m_radarPowerOn = isPowerOn;
}

void RadarIqSensor::setImitatorMark(bool isImitator)
{
    m_isImitator = isImitator;
}

} // namespace mpk::dss::backend::radariq
