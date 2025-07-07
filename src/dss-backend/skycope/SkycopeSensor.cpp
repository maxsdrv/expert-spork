#include "SkycopeSensor.h"

#include "SkycopeController.h"
#include "SkycopeJammer.h"

#include "dss-core/audit/AuditEventLogger.h"

#include "log/Log2.h"

#include <cmath>
#include <optional>
#include <utility>

namespace mpk::dss::backend::skycope
{

constexpr auto sensorDefaultModel = "Tamerlan S-3000";

constexpr double circleAngle = 360.0;
constexpr double directionFindingAccuracy = 7.5;

SkycopeSensor::SkycopeSensor(
    std::shared_ptr<SkycopeConnection> connection,
    const QString& model,
    core::JammerAutoDefenseData autoData,
    gsl::not_null<const core::LicenseController*> licenseController,
    QObject* parent) :
  core::SensorJammerControl(autoData, licenseController),
  QObject(parent),
  m_connection(std::move(connection)),
  m_model(model.isEmpty() ? sensorDefaultModel : model),
  m_positionSignals{std::make_unique<core::DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<core::DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<core::DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<core::DeviceStateSignals>()},
  m_hwInfoSignals{std::make_unique<core::DeviceHwInfoSignals>()}
{
    connect(
        m_connection.get(),
        &SkycopeConnection::systemDataUpdated,
        this,
        &SkycopeSensor::updateSystemData);

    connect(
        m_connection.get(),
        &SkycopeConnection::updateStarted,
        this,
        &SkycopeSensor::updateState);
}

SkycopeSensor::~SkycopeSensor() = default;

core::DeviceType::Value SkycopeSensor::type() const
{
    return core::DeviceType::RFD;
}

auto SkycopeSensor::serial() const -> Serial
{
    return m_serial;
}

auto SkycopeSensor::model() const -> Model
{
    return m_model;
}

auto SkycopeSensor::swVersion() const -> Version
{
    return m_swVersion;
}

auto SkycopeSensor::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_serial;
    }
    return {};
}

core::DevicePosition SkycopeSensor::position() const
{
    return m_positionMode == core::DevicePositionMode::AUTO ? m_autoPosition :
                                                              m_manualPosition;
}

void SkycopeSensor::setPosition(const core::DevicePosition& position)
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

core::DevicePositionMode SkycopeSensor::positionMode() const
{
    return m_positionMode;
}

void SkycopeSensor::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> SkycopeSensor::positionSignals()
    const
{
    return m_positionSignals.get();
}

bool SkycopeSensor::disabled() const
{
    return m_disabled;
}

void SkycopeSensor::setDisabled(bool disabled)
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

gsl::not_null<const core::DeviceDisabledSignals*> SkycopeSensor::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

core::DeviceWorkzone SkycopeSensor::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> SkycopeSensor::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value SkycopeSensor::state() const
{
    return m_state;
}

gsl::not_null<const core::DeviceStateSignals*> SkycopeSensor::stateSignals()
    const
{
    return m_stateSignals.get();
}

core::DeviceHwInfo SkycopeSensor::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> SkycopeSensor::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

std::vector<core::DeviceId> SkycopeSensor::jammerIds() const
{
    std::vector<core::DeviceId> result;
    if (m_jammer)
    {
        result.push_back(m_jammer->first);
    }
    return result;
}

void SkycopeSensor::setJammer(
    const core::DeviceId& id, gsl::not_null<SkycopeJammer*> jammer)
{
    m_jammer = std::make_pair(id, jammer);
}

void SkycopeSensor::updateState(bool connected, bool faultless)
{
    auto state = core::DeviceState::UNAVAILABLE;

    if (connected)
    {
        if (m_deviceConditionOk && faultless)
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
sensors::RfdTargetAttributes targetAttributes(
    const QString& model,
    const QString& modulation,
    const QList<QList<qint32>>& recentFrequencies,
    bool geolocation = false)
{
    std::vector<int> frequencies;
    constexpr int dividerGHzToMHz = 1000;
    for (const auto& frequency: recentFrequencies)
    {
        frequencies.push_back(frequency.at(0) / dividerGHzToMHz);
    }
    return sensors::RfdTargetAttributes{
        .digital = !modulation.contains("analog", Qt::CaseInsensitive),
        .frequencies = frequencies,
        .description = model.toStdString(),
        .geolocation = geolocation};
}

QGeoCoordinate qGeoCoordinate(const OASkycopeGps& gps)
{
    return QGeoCoordinate{gps.getLat(), gps.getLng(), 0};
}

double coordsToBearing(
    const QGeoCoordinate& droneGps, const QGeoCoordinate& devicePos)
{
    const double lat1 = devicePos.latitude();
    const double lon1 = devicePos.longitude();
    const double lat2 = droneGps.latitude();
    const double lon2 = droneGps.longitude();
    // FIXME: 0, 0 = no DF ?
    double bearing = atan2(
        (cos(lat1) * sin(lat2)) - (sin(lat1) * cos(lat2) * cos(lon2 - lon1)),
        sin(lon2 - lon1) * cos(lat2));
    bearing *= circleAngle / M_PI / 2;
    // FIXME:  update formula instead of this
    constexpr auto sqAngle = 90;
    return sqAngle - bearing;
}

constexpr auto typeDrone = "drone";
constexpr auto typeRc = "rc";
constexpr auto lenGeolocationHint = 20;

bool isCoordinateDrone(const OASkycopeDrone& dro)
{
    return dro.getId().size() > lenGeolocationHint;
}

bool isCoordinateDroneConsole(const OASkycopeDrone& dro)
{
    return dro.getId().size() > lenGeolocationHint && dro.is_paired_rc_Set()
           && dro.getPairedRc().getType() == typeRc;
}

template <class DroneClass>
bool isZeroCoord(const DroneClass& device)
{
    return !device.is_gps_Set() || !device.getGps().is_lat_Set()
           || !device.getGps().is_lng_Set()
           || (device.getGps().getLat() == 0 && device.getGps().getLng() == 0);
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void SkycopeSensor::parseDronesInfo(
    const QMap<QString, OASkycopeDrone>& drones, const QString& sensorName)
{
    for (auto it = drones.keyValueBegin(); it != drones.keyValueEnd(); ++it)
    {
        auto id = it->first;
        auto device = it->second;
        LOG_TRACE << "parseDronesInfo[" << id << "]: " << device.asJson();
        auto attributes = targetAttributes(
            device.getModel(),
            device.getModulation(),
            device.getFcKhzAndCountRecent());

        const auto zeroCoord = isZeroCoord(device);
        if (zeroCoord)
        {
            LOG_WARNING << "parseDronesInfo(): drone has zero coordinates";
        }
        LOG_TRACE << "parseDronesInfo(): Is coord drone"
                  << isCoordinateDrone(device);
        if (isCoordinateDrone(device))
        {
            attributes.geolocation = true;
            if (!zeroCoord)
            {
                LOG_TRACE << "parseDronesInfo(): Coord drone not zero";

                emit targetDetected(
                    id,
                    core::TargetClass::DRONE,
                    attributes,
                    correctCoord(qGeoCoordinate(device.getGps())),
                    std::nullopt);
            }
            if (isCoordinateDroneConsole(device)
                && !isZeroCoord(device.getPairedRc()))
            {
                LOG_TRACE << "parseDronesInfo(): Coord console not zero";

                constexpr auto PairedRcSlug = "paired_rc";
                emit targetDetected(
                    id + PairedRcSlug,
                    core::TargetClass::REMOTE_CONSOLE,
                    attributes,
                    correctCoord(qGeoCoordinate(device.getPairedRc().getGps())),
                    std::nullopt);
            }
        }
        else
        {
            parseNormalDrone(id, device, sensorName, attributes);
        }
    }
}

void SkycopeSensor::parseNormalDrone(
    const QString& id,
    const OASkycopeDrone& device,
    const QString& sensorName,
    const sensors::RfdTargetAttributes& attributes)
{
    core::TargetClass::Value targetClass = core::TargetClass::UNDEFINED;
    if (device.getType() == typeDrone)
    {
        targetClass = core::TargetClass::DRONE;
    }
    else if (device.getType() == typeRc)
    {
        targetClass = core::TargetClass::REMOTE_CONSOLE;
    }
    else
    {
        LOG_ERROR << "parseDronesInfo(): Unknown target class";
    }

    auto ld_result = device.getLdResult();
    std::optional<double> bearing = NAN;
    if (ld_result.contains(sensorName))
    {
        LOG_TRACE << "parseDronesInfo(): Drone has bearing";
        bearing = ld_result.value(sensorName).getAzimuth();
    }
    else
    {
        if (isZeroCoord(device))
        {
            LOG_TRACE << "parseDronesInfo(): Normal drone zero coord";
            bearing = std::nullopt;
        }
        else
        {
            LOG_TRACE << "parseDronesInfo(): Normal drone bearing from coord";
            bearing = coordsToBearing(
                qGeoCoordinate(device.getGps()), position().coordinate);
        }
    }
    emit targetDetected(id, targetClass, attributes, std::nullopt, bearing);
}

void SkycopeSensor::parseWifiInfo(
    const QMap<QString, OASkycopeDroneWifiDevice>& consoles)
{
    for (auto it = consoles.keyValueBegin(); it != consoles.keyValueEnd(); ++it)
    {
        auto id = it->first;
        auto device = it->second;
        LOG_TRACE << "parseConsolesInfo[" << id << "]: " << device.asJson();
        auto attributes = targetAttributes(
            device.getModel() + " " + device.getWifiSsid(),
            device.getModulation(),
            device.getFcKhzAndCountRecent());

        emit targetDetected(
            id,
            core::TargetClass::REMOTE_CONSOLE,
            attributes,
            correctCoord(qGeoCoordinate(device.getGps())),
            std::nullopt);
    }
}

QGeoCoordinate SkycopeSensor::correctCoord(const QGeoCoordinate& coord) const
{
    if (m_positionMode == core::DevicePositionMode::AUTO)
    {
        return coord;
    }
    return {
        coord.latitude()
            + (m_manualPosition.coordinate.latitude()
               - m_autoPosition.coordinate.latitude()),
        coord.longitude()
            + (m_manualPosition.coordinate.longitude()
               - m_autoPosition.coordinate.longitude()),
        coord.altitude()
            + (m_manualPosition.coordinate.altitude()
               - m_autoPosition.coordinate.altitude())};
}

void SkycopeSensor::updateSystemData(const OASkycopeModel& content)
{
    auto sensors(content.getSensorsInfo());
    if (sensors.count() < 1)
    {
        LOG_ERROR << "updateSystemData: SensorsInfo is not valid";
        return;
    }

    m_serial = sensors.firstKey();
    const auto& sensor = sensors[m_serial];
    if (!sensor.isValid())
    {
        LOG_ERROR << "updateSystemData: Sensor is not valid";
        return;
    }

    // unset Smart Mode
    // sensor.setIsWidebandEnabled(true);
    // FIXME: broken nobody knows why
    // m_connection->postData(content);

    LOG_TRACE << "Sensor model:" << sensor.getSensorName();

    core::DevicePosition position;
    {
        // TODO: protect all dynamic vars
        m_swVersion = sensor.getMgmtDetails().getModule1().getSoftwareVersion();
        position = core::DevicePosition{
            .azimuth = static_cast<double>(sensor.getCompass()),
            .coordinate = qGeoCoordinate(sensor.getGps())};

        auto oldAutoPosition = m_autoPosition;
        m_autoPosition = position;
        if (m_positionMode == core::DevicePositionMode::AUTO
            && position != oldAutoPosition)
        {
            emit m_positionSignals->positionChanged();
        }
    }

    const double distance = sensor.getDefenseRadius();
    const int sectorsCount =
        static_cast<int>(round(circleAngle / directionFindingAccuracy));
    const auto sectorWidthAngle = circleAngle / sectorsCount;

    auto workzone = core::DeviceWorkzone{};
    for (int sectorNumber = 0; sectorNumber < sectorsCount; sectorNumber++)
    {
        const double minAngle = sectorNumber * sectorWidthAngle;
        const double maxAngle = (sectorNumber + 1) * sectorWidthAngle;
        workzone.sectors.emplace_back(core::DeviceWorkzoneSector{
            sectorNumber, distance, minAngle, maxAngle});
    }

    if (workzone != m_workzone)
    {
        m_workzone = workzone;
        emit m_workzoneSignals->workzoneChanged(m_workzone);
    }

    m_deviceConditionOk =
        sensor.getWifiStatus().getValue()
            == OASkycope::OASkycopeWifiStatus::eOASkycopeWifiStatus::CONNECTED
        && sensor.getMcuStatus().getValue()
               == OASkycope::OASkycopeMcuStatus::eOASkycopeMcuStatus::CONNECTED;

    if (!m_disabled)
    {
        parseDronesInfo(content.getDevicesInfo(), sensor.getSensorName());
    }
}

} // namespace mpk::dss::backend::skycope
