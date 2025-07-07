#include "SkycopeController.h"

#include "SkycopeSensor.h"

#include "dss-backend/BackendComponents.h"
#include "dss-backend/BackendSignals.h"
#include "dss-backend/skycope/SkycopeJammer.h"
#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/ConnectionSettings.h"
#include "dss-common/config/HttpTags.h"
#include "dss-common/config/Option.h"
#include "dss-core/AlarmStatus.h"
#include "dss-core/SensorDevice.h"
#include "dss-core/YamlConvertHelper.h"
#include "dss-core/audit/AuditEventLogger.h"

#include <QUuid>

#include <chrono>

namespace mpk::dss::backend::skycope
{

namespace
{
using namespace std::chrono_literals;
constexpr auto trackLifetime = 15s;

constexpr auto timeoutTag = "timeout";
constexpr auto updateIntervalTag = "update_interval";
constexpr auto jammerDistanceTag = "jammer_distance";
constexpr auto modelTag = "model";
constexpr auto jammerModelTag = "jammer_model";

} // namespace

BackendControllerPtr SkycopeController::fromDescription(
    const YAML::Node& skycopeNode, const BackendComponents& components)
{
    auto httpConnectionSettings = core::convertYAML<config::ConnectionSettings>(
        skycopeNode[config::httpTag]);
    auto updateIntervalSettings =
        core::convertYAML<config::Option<int>>(skycopeNode[updateIntervalTag]);
    auto timeoutSettings =
        core::convertYAML<config::Option<int>>(skycopeNode[timeoutTag]);
    auto authSettings =
        core::convertYAML<config::AuthSettings>(skycopeNode[config::authTag]);
    auto jammerDistance =
        core::convertYAML<config::Option<int>>(skycopeNode[jammerDistanceTag]);
    QString sensorModel;
    auto sensorModelNode = skycopeNode[modelTag];
    if (sensorModelNode.IsDefined() && !sensorModelNode.IsNull())
    {
        auto modelStr =
            core::convertYAML<config::Option<std::string>>(sensorModelNode);
        sensorModel = QString::fromStdString(modelStr.value);
    }
    QString jammerModel;
    auto jammerModelNode = skycopeNode[jammerModelTag];
    if (jammerModelNode.IsDefined() && !jammerModelNode.IsNull())
    {
        auto modelStr =
            core::convertYAML<config::Option<std::string>>(jammerModelNode);
        jammerModel = QString::fromStdString(modelStr.value);
    }

    auto connection = std::make_shared<SkycopeConnection>(
        httpConnectionSettings,
        std::chrono::milliseconds(updateIntervalSettings.value),
        std::chrono::milliseconds(timeoutSettings.value),
        authSettings);

    return std::unique_ptr<SkycopeController>(new SkycopeController{
        std::move(connection),
        jammerDistance.value,
        sensorModel,
        jammerModel,
        components});
}

SkycopeController::SkycopeController(
    std::shared_ptr<SkycopeConnection> connection,
    int jammerDistance,
    const QString& sensorModel,
    const QString& jammerModel,
    const BackendComponents& components,
    QObject* parent) :
  QObject{parent},
  BackendControllerBase{std::make_unique<BackendSignals>(), components},
  m_connection{std::move(connection)}
{
    auto sensor = std::make_unique<SkycopeSensor>(
        m_connection,
        sensorModel,
        core::JammerAutoDefenseData{jammers(), pin()},
        components.licenseController,
        this);

    connect(
        sensor.get(),
        &SkycopeSensor::targetDetected,
        this,
        &SkycopeController::sendTarget);

    auto jammer = std::make_unique<SkycopeJammer>(
        m_connection,
        sensor.get(),
        jammerDistance,
        jammerModel,
        std::nullopt,
        this);

    QObject::connect(
        jammers()->pin(),
        &core::DeviceProviderSignals::registered,
        jammers()->pin(),
        [this, sensorPtr = sensor.get()](const auto& deviceId)
        {
            auto jammers = this->jammers();
            auto it = jammers->find(deviceId);
            Ensures(it != jammers->end());

            // Looks like dirty hackery
            auto* skycopeJammer =
                dynamic_cast<SkycopeJammer*>(&it->properties());
            Ensures(skycopeJammer);

            connect(
                skycopeJammer->disabledSignals(),
                &core::DeviceDisabledSignals::disabledChanged,
                this,
                [this](bool disabled) { jammerDisabled(disabled); });

            // TODO: sensor may be not registered yet!
            sensorPtr->setJammer(deviceId, skycopeJammer);
        });

    QObject::connect(
        sensors()->pin(),
        &core::DeviceProviderSignals::registered,
        sensors()->pin(),
        [this, jammerPtr = jammer.get()](const auto& deviceId)
        {
            m_sensorId = deviceId;

            auto sensors = this->sensors();
            auto it = sensors->find(deviceId);
            Ensures(it != sensors->end());

            // Looks like dirty hackery
            auto* skycopeSensor =
                dynamic_cast<SkycopeSensor*>(&it->properties());
            Ensures(skycopeSensor);

            connect(
                skycopeSensor->disabledSignals(),
                &core::DeviceDisabledSignals::disabledChanged,
                this,
                &SkycopeController::sensorDisabled);

            jammerPtr->setSensorId(deviceId);
        });

    m_sensor = sensor.get();
    m_jammer = jammer.get();
    appendSensor(std::move(sensor));
    appendJammer(std::move(jammer));
}

void SkycopeController::sensorDisabled(bool disabled)
{
    m_sensorDisabled = disabled;
    m_connection->setUpdateDisabled(m_sensorDisabled && m_jammerDisabled);
}

void SkycopeController::jammerDisabled(bool disabled)
{
    m_jammerDisabled = disabled;
    m_connection->setUpdateDisabled(m_sensorDisabled && m_jammerDisabled);
}

void SkycopeController::sendTarget(
    const QString& originId,
    core::TargetClass::Value targetClass,
    const sensors::RfdTargetAttributes& attributes,
    const std::optional<QGeoCoordinate>& coordinate,
    std::optional<double> bearing)
{
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [originId](const auto& trackData)
        { return trackData.originId == originId; });
    if (it == m_trackData.end())
    {
        auto trackId = core::TrackId::generate();
        if (QString(m_sensorId).isEmpty())
        {
            LOG_WARNING
                << "SkycopeController::sendTarget(): waiting for sensor ID, alarm not sent";
            return;
        }

        it = m_trackData.insert(
            it, {m_sensorId, trackId, originId, std::make_unique<QTimer>()});
        core::AlarmStatus alarmStatus = {
            .level = core::AlarmLevel::CRITICAL, .score = 0};
        LOG_DEBUG << "SkycopeController: track started" << it->trackId;
        emit pin()->trackStarted(
            it->deviceId,
            it->trackId,
            {},
            alarmStatus,
            core::TargetClass::DRONE,
            json::toValue(attributes).toObject());

        QObject::connect(
            it->timer.get(),
            &QTimer::timeout,
            it->timer.get(),
            [this, trackId]() { removeTrack(trackId); });

        core::auditLogger().logAlarm(
            alarmStatus.level,
            core::DeviceClass::toString(m_sensor->deviceClass),
            core::DeviceType::toString(m_sensor->type()),
            nodeKey,
            m_sensor->serial());
    }
    it->timer->start(trackLifetime);

    auto trackPoint = core::TrackPoint{
        .coordinate = coordinate,
        .direction =
            bearing.has_value() ? std::optional<core::Direction>(core::Direction{
                .bearing = bearing.value(), .distance = 0, .elevation = 0}) :
                                  std::nullopt,
        .timestamp = QDateTime::currentDateTimeUtc(),
        .rawData = {}};
    LOG_DEBUG << "SkycopeController: track updated" << it->trackId;
    emit pin()->trackUpdated(
        it->deviceId,
        it->trackId,
        {.level = core::AlarmLevel::CRITICAL, .score = 0},
        targetClass,
        json::toValue(attributes).toObject(),
        {trackPoint});
}

void SkycopeController::removeTrack(const core::TrackId& trackId)
{
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [trackId](const auto& trackData)
        { return trackData.trackId == trackId; });
    if (it != m_trackData.end())
    {
        LOG_DEBUG << "SkycopeController: track finished " << it->trackId;
        emit pin()->trackFinished(it->deviceId, it->trackId);

        m_trackData.erase(it);
    }
    else
    {
        LOG_WARNING << "SkycopeController: track " << trackId
                    << " not found for removal";
    }
}

} // namespace mpk::dss::backend::skycope
