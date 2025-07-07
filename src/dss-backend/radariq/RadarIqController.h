#pragma once

#include "RadarIqCamera.h"
#include "RadarIqCameraControl.h"
#include "RadarIqConnection.h"
#include "RadarIqSensor.h"

#include "dss-backend/BackendControllerBase.h"
#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/ConnectionSettings.h"

#include <yaml-cpp/yaml.h>

#include <QObject>
#include <QTimer>

#include <memory>
#include <utility>

namespace mpk::dss::backend::radariq
{
using namespace OAService;
using namespace OARadarIq;

constexpr auto nodeKey = "radariq";

class RadarIqController final : public QObject, public BackendControllerBase
{
    Q_OBJECT
public:
    static BackendControllerPtr fromDescription(
        const YAML::Node& radarIqNode, const BackendComponents& components);

private:
    RadarIqController(
        const BackendComponents& components,
        const config::ConnectionSettings& httpConnectionSettings,
        const config::ConnectionSettings& wsConnectionSettings,
        const std::chrono::milliseconds& updateIntervalSettings,
        const std::chrono::milliseconds& timeoutSettings,
        const config::AuthSettings& authSettings,
        const QString& radarModel,
        const QString& cameraModel,
        int cameraDistance,
        QObject* parent = nullptr);

    void registerHandlers();

    [[nodiscard]] auto getDeviceId(bool isRadar) const
        -> std::pair<bool, core::DeviceId>;
    void finishTrack(const core::TrackId& trackId);

    [[nodiscard]] QGeoCoordinate correctRadarCoord(
        const QGeoCoordinate& coord) const;

private slots:
    void disableRadar(bool disabled);
    void disableCamera(bool disabled);

    void updateTrack(bool isRadar, const OARadarIqTrack& track);
    void removeTrack(const OARadarIqTrack_id& trackId);

private:
    std::shared_ptr<RadarIqConnection> m_connection;

    RadarIqSensor* m_radar;
    core::DeviceId m_radarId;
    bool m_radarDisabled = false;

    RadarIqCamera* m_camera;
    RadarIqCameraControl* m_cameraControl;
    core::DeviceId m_cameraId;
    bool m_cameraDisabled = false;

    using OriginTrackId = int;
    struct TrackData
    {
        core::DeviceId deviceId;
        core::TrackId trackId;
        OriginTrackId originId;
        std::unique_ptr<QTimer> timer;
    };
    std::vector<TrackData> m_trackData;
};

} // namespace mpk::dss::backend::radariq
