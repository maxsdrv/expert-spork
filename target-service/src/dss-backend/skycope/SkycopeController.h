#pragma once

#include "SkycopeConnection.h"
#include "SkycopeJammer.h"
#include "dss-backend/BackendControllerBase.h"
#include "dss-common/sensors/RfdTargetAttributes.h"

#include <yaml-cpp/yaml.h>

#include <QObject>

#include <memory>
#include <unordered_map>

namespace mpk::dss::backend::skycope
{

constexpr auto nodeKey = "skycope";

class SkycopeController : public QObject, public BackendControllerBase
{
    Q_OBJECT

public:
    static BackendControllerPtr fromDescription(
        const YAML::Node& node, const BackendComponents& components);

private:
    explicit SkycopeController(
        std::shared_ptr<SkycopeConnection> connection,
        int jammerDistance,
        const QString& sensorModel,
        const QString& jammerModel,
        const BackendComponents& components,
        QObject* parent = nullptr);

    void removeTrack(const core::TrackId& trackId);

private slots:
    void sensorDisabled(bool disabled);
    void jammerDisabled(bool disabled);

    void sendTarget(
        const QString& id,
        core::TargetClass::Value targetClass,
        const sensors::RfdTargetAttributes& attributes,
        const std::optional<QGeoCoordinate>& coordinate,
        std::optional<double> bearing,
        const qint64& detectCount);

private:
    std::shared_ptr<SkycopeConnection> m_connection;

    SkycopeSensor* m_sensor;
    core::DeviceId m_sensorId;
    bool m_sensorDisabled = false;

    SkycopeJammer* m_jammer;
    core::DeviceId m_jammerId;
    bool m_jammerDisabled = false;

    using OriginTrackId = QString;
    struct TrackData
    {
        core::DeviceId deviceId;
        core::TrackId trackId;
        OriginTrackId originId;
        std::unique_ptr<QTimer> timer;
    };

    std::vector<TrackData> m_trackData;
};

} // namespace mpk::dss::backend::skycope
