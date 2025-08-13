#include "RadarIqController.h"

#include "dss-backend/BackendSignals.h"
#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/HttpTags.h"
#include "dss-common/config/Option.h"
#include "dss-core/AlarmStatus.h"
#include "dss-core/CameraDevice.h"
#include "dss-core/DevicePosition.h"
#include "dss-core/SensorDevice.h"
#include "dss-core/TargetClass.h"
#include "dss-core/YamlConvertHelper.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "OAServiceCamera_track.h"

#include <QList>
#include <QNetworkReply>
#include <QObject>
#include <QUuid>

#include <chrono>

namespace mpk::dss::backend::radariq
{

using namespace std::chrono_literals;
constexpr auto trackLifetime = 15s;

namespace
{

constexpr auto timeoutTag = "timeout";
constexpr auto updateIntervalTag = "update_interval";
constexpr auto radarModelTag = "radar_model";
constexpr auto cameraModelTag = "camera_model";
constexpr auto cameraDistanceTag = "camera_distance";

} // namespace

BackendControllerPtr RadarIqController::fromDescription(
    const YAML::Node& radarIqNode, const BackendComponents& components)
{
    auto httpConnectionSettings = core::convertYAML<config::ConnectionSettings>(
        radarIqNode[config::httpTag]);
    auto updateIntervalSettings =
        core::convertYAML<config::Option<int>>(radarIqNode[updateIntervalTag]);
    auto timeoutSettings =
        core::convertYAML<config::Option<int>>(radarIqNode[timeoutTag]);
    auto wsConnectionSettings = core::convertYAML<config::ConnectionSettings>(
        radarIqNode[config::wsTag]);
    auto authSettings =
        core::convertYAML<config::AuthSettings>(radarIqNode[config::authTag]);

    QString radarModel;
    auto radarModelNode = radarIqNode[radarModelTag];
    if (radarModelNode.IsDefined() && !radarModelNode.IsNull())
    {
        auto modelStr =
            core::convertYAML<config::Option<std::string>>(radarModelNode);
        radarModel = QString::fromStdString(modelStr.value);
    }
    QString cameraModel;
    auto cameraModelNode = radarIqNode[cameraModelTag];
    if (cameraModelNode.IsDefined() && !cameraModelNode.IsNull())
    {
        auto modelStr =
            core::convertYAML<config::Option<std::string>>(cameraModelNode);
        cameraModel = QString::fromStdString(modelStr.value);
    }
    auto cameraDistance =
        core::convertYAML<config::Option<int>>(radarIqNode[cameraDistanceTag]);

    auto controller = std::unique_ptr<RadarIqController>(new RadarIqController{
        components,
        httpConnectionSettings,
        wsConnectionSettings,
        std::chrono::milliseconds(updateIntervalSettings.value),
        std::chrono::milliseconds(timeoutSettings.value),
        authSettings,
        radarModel,
        cameraModel,
        cameraDistance.value});

    return controller;
}

RadarIqController::RadarIqController(
    const BackendComponents& components,
    const config::ConnectionSettings& httpConnectionSettings,
    const config::ConnectionSettings& wsConnectionSettings,
    const std::chrono::milliseconds& updateIntervalSettings,
    const std::chrono::milliseconds& timeoutSettings,
    const config::AuthSettings& authSettings,
    const QString& radarModel,
    const QString& cameraModel,
    int cameraDistance,
    QObject* parent) :
  QObject{parent},
  BackendControllerBase(std::make_unique<BackendSignals>(), components),
  m_connection(std::make_shared<RadarIqConnection>(
      httpConnectionSettings,
      wsConnectionSettings,
      updateIntervalSettings,
      timeoutSettings,
      authSettings,
      this)),
  m_radar(new RadarIqSensor(m_connection, radarModel, this)),
  m_camera(new RadarIqCamera(m_connection, cameraModel, cameraDistance, this)),
  m_cameraControl(new RadarIqCameraControl(httpConnectionSettings.host, this))
{
    appendSensor(std::unique_ptr<RadarIqSensor>(m_radar));
    appendSensor(std::unique_ptr<RadarIqCamera>(m_camera));

    QObject::connect(
        sensors()->pin(),
        &core::DeviceProviderSignals::registered,
        sensors()->pin(),
        [this](const auto& deviceId)
        {
            auto sensors = this->sensors();
            auto it = sensors->find(deviceId);
            Ensures(it != sensors->end());
            if (&it->properties() == m_camera)
            {
                appendCamera(
                    deviceId,
                    std::unique_ptr<RadarIqCameraControl>(m_cameraControl));
                m_cameraId = deviceId;
                LOG_DEBUG << "Radar IQ: set camera device id: " << m_cameraId;
            }
            else if (&it->properties() == m_radar)
            {
                m_radarId = deviceId;
                LOG_DEBUG << "Radar IQ: set radar device id: " << m_radarId;
            }
            else
            {
                LOG_ERROR << "Radar IQ: unknown device id: " << deviceId;
            }
        });

    registerHandlers();
}

void RadarIqController::registerHandlers()
{
    connect(
        m_radar->disabledSignals(),
        &core::DeviceDisabledSignals::disabledChanged,
        this,
        &RadarIqController::disableRadar);
    connect(
        m_camera->disabledSignals(),
        &core::DeviceDisabledSignals::disabledChanged,
        this,
        &RadarIqController::disableCamera);
    connect(
        m_connection.get(),
        &RadarIqConnection::trackUpdated,
        this,
        &RadarIqController::updateTrack);
    connect(
        m_connection.get(),
        &RadarIqConnection::trackRemoved,
        this,
        &RadarIqController::removeTrack);
    connect(
        m_connection.get(),
        &RadarIqConnection::imitatorDetected,
        m_radar,
        &RadarIqSensor::setImitatorMark);
    connect(
        m_connection.get(),
        &RadarIqConnection::imitatorDetected,
        m_camera,
        &RadarIqCamera::setImitatorMark);
}

void RadarIqController::disableRadar(bool disabled)
{
    m_radarDisabled = disabled;
    m_connection->setDisabled(m_radarDisabled && m_cameraDisabled);
}

void RadarIqController::disableCamera(bool disabled)
{
    m_cameraDisabled = disabled;
    m_connection->setDisabled(m_radarDisabled && m_cameraDisabled);
}

auto RadarIqController::getDeviceId(bool isRadar) const
    -> std::pair<bool, core::DeviceId>
{
    if (isRadar)
    {
        if (!m_radarDisabled)
        {
            return std::make_pair(true, m_radarId);
        }
    }
    else
    {
        if (!m_cameraDisabled)
        {
            return std::make_pair(true, m_cameraId);
        }
    }
    return std::make_pair(false, core::DeviceId{});
}

void RadarIqController::updateTrack(bool isRadar, const OARadarIqTrack& track)
{
    auto [enabled, deviceId] = getDeviceId(isRadar);
    if (!enabled)
    {
        return;
    }

    core::TargetClass::Value className = core::TargetClass::UNDEFINED;
    try
    {
        // direct conversion from radar's enum
        className =
            core::TargetClass::fromString(track.getObjectClass().asJson());
    }
    catch (exception::StringParseFailed<QString>&)
    {
        LOG_WARNING << "Error parsing radar object class";
    }

    core::AlarmStatus alarmStatus = {
        .level = core::AlarmLevel::UNDEFINED,
        .score = track.getPriorityScore()};
    try
    {
        // direct conversion from radar's enum
        alarmStatus.level =
            core::AlarmLevel::fromString(track.getAlarmLevel().asJson());
    }
    catch (exception::StringParseFailed<QString>&)
    {
        LOG_WARNING << "Error parsing radar alarm level";
    }

    core::TrackPoints trackPoints;
    for (const auto constPoints = track.getPoints();
         const auto& point: constPoints)
    {
        if (point.is_filter_Set() && point.getFilter() != 0)
        {
            continue;
        }

        if (isRadar)
        {
            trackPoints.emplace_back(core::TrackPoint{
                .coordinate = correctRadarCoord(QGeoCoordinate{
                    point.getCoord().getLatitude(),
                    point.getCoord().getLongitude(),
                    point.getCoord().getAltitude()}),
                .direction = std::nullopt,
                .timestamp = point.getTimestamp(),
                .rawData = point.asJsonObject()});
        }
        else
        {
            trackPoints.emplace_back(core::TrackPoint{
                .coordinate = std::nullopt,
                .direction =
                    core::Direction{
                        .bearing = point.getGeometry().getAzimuth()
                                   + m_camera->position().azimuth,
                        .distance = point.getGeometry().getDistance(),
                        .elevation = point.getGeometry().getElevation()},
                .timestamp = point.getTimestamp(),
                .rawData = point.asJsonObject()});
        }
    }

    auto originId = track.getId().getSource();
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [originId, &deviceId = deviceId](const auto& trackData) {
            return trackData.originId == originId
                   && trackData.deviceId == deviceId;
        });

    if (it == m_trackData.end())
    {
        if (trackPoints.empty())
        {
            LOG_DEBUG << "Radar IQ filtered new target: " << originId;
            return;
        }

        auto trackId = core::TrackId::generate();
        it = m_trackData.insert(
            it,
            TrackData{deviceId, trackId, originId, std::make_unique<QTimer>()});

        const auto deviceClass =
            isRadar ? m_radar->deviceClass : m_camera->deviceClass;
        const auto deviceType = core::DeviceType::toString(
            isRadar ? m_radar->type() : m_camera->type());
        const auto serial = isRadar ? m_radar->serial() : m_camera->serial();
        core::auditLogger().logAlarm(
            alarmStatus.level,
            core::DeviceClass::toString(deviceClass),
            deviceType,
            nodeKey,
            serial);

        OAServiceCamera_track cameraTrack;
        cameraTrack.setTargetId(QString::number(originId));
        cameraTrack.setCameraId(m_cameraControl->id());

        LOG_DEBUG << "Radar IQ controller: track started" << it->trackId;
        emit pin()->trackStarted(
            it->deviceId, it->trackId, cameraTrack, alarmStatus, className, {});

        QObject::connect(
            it->timer.get(),
            &QTimer::timeout,
            it->timer.get(),
            [this, trackId]() { finishTrack(trackId); });
    }
    it->timer->start(trackLifetime);

    if (!trackPoints.empty())
    {
        LOG_DEBUG << "Radar IQ controller: track updated " << it->trackId;
        emit pin()->trackUpdated(
            it->deviceId,
            it->trackId,
            alarmStatus,
            className,
            {},
            trackPoints,
            ++it->detectCount);
    }
    else
    {
        LOG_DEBUG << "Radar IQ filtered existing target: " << originId;
    }
}

void RadarIqController::removeTrack(const OARadarIqTrack_id& trackId)
{
    auto originId = trackId.getSource();
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [originId](const auto& trackData)
        { return trackData.originId == originId; });

    if (it != m_trackData.end())
    {
        finishTrack(it->trackId);
    }
    else
    {
        LOG_DEBUG
            << "Radar IQ controller: track not found for track remove message source id: "
            << originId << " (probably filtered)";
    }
}

void RadarIqController::finishTrack(const core::TrackId& trackId)
{
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [trackId](const auto& trackData)
        { return trackData.trackId == trackId; });
    if (it != m_trackData.end())
    {
        LOG_DEBUG << "Radar IQ controller: track finished " << it->trackId;
        emit pin()->trackFinished(it->deviceId, it->trackId);

        m_trackData.erase(it);
    }
    else
    {
        LOG_WARNING << "Radar IQ controller: track " << trackId
                    << " not found for removal";
    }
}

QGeoCoordinate RadarIqController::correctRadarCoord(
    const QGeoCoordinate& coord) const
{
    if (m_radar->positionMode() == core::DevicePositionMode::AUTO)
    {
        return coord;
    }
    return {
        coord.latitude()
            + (m_radar->manualPosition().coordinate.latitude()
               - m_radar->autoPosition().coordinate.latitude()),
        coord.longitude()
            + (m_radar->manualPosition().coordinate.longitude()
               - m_radar->autoPosition().coordinate.longitude()),
        coord.altitude()
            + (m_radar->manualPosition().coordinate.altitude()
               - m_radar->autoPosition().coordinate.altitude())};
}

} // namespace mpk::dss::backend::radariq
