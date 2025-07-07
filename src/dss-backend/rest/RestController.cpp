#include "RestController.h"

#include "HttpWsSensor.h"

#include "json/SensorTargetDataVsJson.h"

#include "dss-backend/BackendSignals.h"
#include "dss-common/config/ConnectionSettings.h"
#include "dss-common/config/HttpTags.h"
#include "dss-common/config/Option.h"
#include "dss-common/sensors/RfdTargetAttributes.h"
#include "dss-core/YamlConvertHelper.h"

#include "httpws/ws/WebSocketClient.h"
#include "log/Log2.h"

#include "gsl/pointers"

#include <chrono>

namespace mpk::dss::backend::rest
{

namespace
{

using namespace std::chrono_literals;

constexpr auto trackLifetime = 15s;

constexpr auto sensorsTag = "sensors";
constexpr auto timeoutTag = "timeout";
constexpr auto updateIntervalTag = "update_interval";

} // namespace

BackendControllerPtr RestController::fromDescription(
    const YAML::Node& description, const BackendComponents& components)
{
    auto connections =
        std::vector<std::unique_ptr<mpk::rest::HttpWsConnection>>{};
    auto updateIntervals = std::vector<int>{};
    auto timeouts = std::vector<int>{};

    auto sensorsNode = description[sensorsTag];
    if (!sensorsNode.IsNull() && sensorsNode.size() > 0)
    {
        auto sensorNodes = sensorsNode.as<std::vector<YAML::Node>>();
        for (const auto& sensorNode: sensorNodes)
        {
            auto httpConnectionSettings =
                core::convertYAML<config::ConnectionSettings>(
                    sensorNode[config::httpTag]);
            auto updateIntervalSettings =
                core::convertYAML<config::Option<int>>(
                    sensorNode[updateIntervalTag]);
            auto timeoutSettings =
                core::convertYAML<config::Option<int>>(sensorNode[timeoutTag]);
            auto wsConnectionSettings =
                core::convertYAML<config::ConnectionSettings>(
                    sensorNode[config::wsTag]);

            connections.push_back(std::make_unique<mpk::rest::HttpWsConnection>(
                httpConnectionSettings,
                wsConnectionSettings,
                QUuid::createUuid().toString()));
            updateIntervals.push_back(updateIntervalSettings.value);
            timeouts.push_back(timeoutSettings.value);
        }
    }

    Ensures(connections.size() == updateIntervals.size());
    Ensures(connections.size() == timeouts.size());

    auto controller =
        std::unique_ptr<RestController>(new RestController{components});

    for (auto n = 0U; n < connections.size(); n++)
    {
        auto sensor = std::make_unique<HttpWsSensor>(
            std::forward<std::unique_ptr<mpk::rest::HttpWsConnection>>(
                connections[n]),
            std::chrono::milliseconds(updateIntervals[n]),
            std::chrono::milliseconds(timeouts[n]));
        controller->appendSensor(std::move(sensor));
    }

    return controller;
}

RestController::RestController(const BackendComponents& components) :
  BackendControllerBase(std::make_unique<BackendSignals>(), components)
{
    QObject::connect(
        sensors()->pin(),
        &core::DeviceProviderSignals::registered,
        sensors()->pin(),
        [this](const auto& deviceId)
        {
            auto sensors = this->sensors();
            auto it = sensors->find(deviceId);
            Ensures(it != sensors->end());

            // Looks like dirty hackery
            auto* httpWsSensor = dynamic_cast<HttpWsSensor*>(&it->properties());
            Ensures(httpWsSensor);

            httpWsSensor->connection()->webSocketClient().registerHandler(
                SensorTargetData::defaultType,
                [this, deviceId](const QJsonObject& json)
                { handleWsMessage(deviceId, json); });
        });
}

void RestController::handleWsMessage(
    const core::DeviceId& deviceId, const QJsonObject& json)
{
    auto sensorTargetData = json::fromValue<SensorTargetData>(json);
    auto originId = sensorTargetData.id;
    const auto attributes =
        json::toValue(sensors::RfdTargetAttributes{
                          .digital = true,
                          .frequencies =
                              {sensorTargetData.frequencies.begin(),
                               sensorTargetData.frequencies.end()},
                          .description = sensorTargetData.description})
            .toObject();
    const auto alarmStatus =
        core::AlarmStatus{.level = core::AlarmLevel::CRITICAL, .score = 0};

    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [originId](const auto& trackData)
        { return trackData.originId == originId; });
    if (it == m_trackData.end())
    {
        auto trackId = core::TrackId::generate();
        it = m_trackData.insert(
            it, {deviceId, trackId, originId, std::make_unique<QTimer>()});
        LOG_DEBUG << "Rest controller: track started" << it->trackId;
        emit pin()->trackStarted(
            it->deviceId,
            it->trackId,
            {},
            alarmStatus,
            core::TargetClass::DRONE,
            attributes);

        QObject::connect(
            it->timer.get(),
            &QTimer::timeout,
            it->timer.get(),
            [this, trackId]() { removeTrack(trackId); });
    }
    it->timer->start(trackLifetime);

    auto trackPoint = core::TrackPoint{
        .coordinate = sensorTargetData.coordinate,
        .direction =
            core::Direction{
                .bearing =
                    sensorTargetData.bearing ? *sensorTargetData.bearing : 0.,
                .distance =
                    sensorTargetData.distance ? *sensorTargetData.distance : 0.,
                .elevation = 0},
        .timestamp = sensorTargetData.timestamp,
        .rawData = json};
    LOG_DEBUG << "Rest controller: track updated" << it->trackId;
    emit pin()->trackUpdated(
        it->deviceId,
        it->trackId,
        alarmStatus,
        core::TargetClass::DRONE,
        attributes,
        {trackPoint});
}

void RestController::removeTrack(const core::TrackId& trackId)
{
    auto it = std::find_if(
        m_trackData.begin(),
        m_trackData.end(),
        [trackId](const auto& trackData)
        { return trackData.trackId == trackId; });
    if (it != m_trackData.end())
    {
        LOG_DEBUG << "Rest controller: track finished " << it->trackId;
        emit pin()->trackFinished(it->deviceId, it->trackId);

        m_trackData.erase(it);
    }
    else
    {
        LOG_WARNING << "Rest controller: track " << trackId
                    << " not found for removal";
    }
}

} // namespace mpk::dss::backend::rest
