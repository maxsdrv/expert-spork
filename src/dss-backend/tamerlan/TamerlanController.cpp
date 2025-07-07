#include "TamerlanController.h"

#include "TamerlanJammer.h"
#include "TamerlanSensor.h"

#include "modbus/AsyncModbusCommandExecutor.h"
#include "modbus/ModbusConnection.h"

#include "dss-backend/BackendComponents.h"
#include "dss-backend/BackendSignals.h"
#include "dss-common/config/Option.h"
#include "dss-core/DeviceId.h"
#include "dss-core/YamlConvertHelper.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "exceptions/InvalidConfig.h"

#include <QString>
#include <QThread>
#include <QUuid>

#include <memory>

namespace mpk::dss::backend::tamerlan
{

namespace
{

using ConnectionId = std::string;
using SensorNumber = int;

constexpr auto connectionsTag = "connections";
constexpr auto idTag = "id";
constexpr auto typeTag = "type";
constexpr auto nameTag = "name";
constexpr auto baudRateTag = "baud_rate";
constexpr auto parityTag = "parity";
constexpr auto dataBitsTag = "data_bits";
constexpr auto stopBitsTag = "stop_bits";
constexpr auto timeoutTag = "timeout";

constexpr auto sensorsTag = "sensors";
constexpr auto jammersTag = "jammers";
constexpr auto numberTag = "number";
constexpr auto modbusAddressTag = "modbus_address";
constexpr auto updateIntervalTag = "update_interval";
constexpr auto sensorUpdateIntervalTag = "sensor_update_interval";
constexpr auto objectsUpdateIntervalTag = "objects_update_interval";
constexpr auto connectionTag = "connection";
constexpr auto sensorTag = "sensor";
constexpr auto channelsTag = "channels";
constexpr auto modelTag = "model";
constexpr auto distanceTag = "distance";
constexpr auto groupIdTag = "group_id";

} // namespace

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
BackendControllerPtr TamerlanController::fromDescription(
    const YAML::Node& description, const BackendComponents& components)
{
    std::map<ConnectionId, std::shared_ptr<AsyncModbusCommandExecutor>>
        executors;
    auto connectionsNode = description[connectionsTag];
    if (!connectionsNode.IsNull() && connectionsNode.size() > 0)
    {
        auto connectionNodes = connectionsNode.as<std::vector<YAML::Node>>();
        for (const auto& connectionNode: connectionNodes)
        {
            auto type = connectionNode[typeTag].as<std::string>();

            auto timeoutSettings = core::convertYAML<config::Option<int>>(
                connectionNode[timeoutTag]);
            std::unique_ptr<ModbusConnection> connection;
            if (type == "tcp")
            {
                connection = std::make_unique<ModbusConnection>(
                    core::convertYAML<config::ConnectionSettings>(
                        connectionNode),
                    std::chrono::milliseconds(timeoutSettings.value));
            }
            else if (type == "rtu")
            {
                connection = std::make_unique<ModbusConnection>(
                    tamerlan::ModbusRtuSettings{
                        .name = core::convertYAML<config::Option<std::string>>(
                                    connectionNode[nameTag])
                                    .value,
                        .baudRate = core::convertYAML<config::Option<int>>(
                                        connectionNode[baudRateTag])
                                        .value,
                        .parity = core::convertYAML<config::Option<char>>(
                                      connectionNode[parityTag])
                                      .value,
                        .dataBits = core::convertYAML<config::Option<int>>(
                                        connectionNode[dataBitsTag])
                                        .value,
                        .stopBits = core::convertYAML<config::Option<int>>(
                                        connectionNode[stopBitsTag])
                                        .value},
                    std::chrono::milliseconds(timeoutSettings.value));
            }
            else
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Invalid modbus connection type: ")
                        + type));
            }
            auto id = core::convertYAML<config::Option<ConnectionId>>(
                connectionNode[idTag]);
            if (executors.find(id.value) != executors.end())
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Duplicate id for connection: ")
                        + id.value));
            }
            auto executor = std::make_shared<AsyncModbusCommandExecutor>(
                std::move(connection));
            auto* executorThread = new QThread();
            executor->moveToThread(executorThread);
            executorThread->start();
            QObject::connect(
                executor.get(),
                &QObject::destroyed,
                executorThread,
                [executorThread]()
                {
                    executorThread->quit();
                    executorThread->wait();
                    executorThread->deleteLater();
                });

            executors.insert({id.value, std::move(executor)});
        }
    }

    std::map<SensorNumber, gsl::not_null<TamerlanSensor*>> sensorsMap;
    std::vector<SensorJammersConnection> sensorsJammersConnection;

    auto controller =
        std::unique_ptr<TamerlanController>(new TamerlanController{components});

    auto sensorsNode = description[sensorsTag];
    if (!sensorsNode.IsNull() && sensorsNode.size() > 0)
    {
        auto sensorNodes = sensorsNode.as<std::vector<YAML::Node>>();
        for (const auto& sensorNode: sensorNodes)
        {
            auto modbusAddress = core::convertYAML<config::Option<int>>(
                sensorNode[modbusAddressTag]);
            auto sensorUpdateInterval = core::convertYAML<config::Option<int>>(
                sensorNode[sensorUpdateIntervalTag]);
            auto objectsUpdateInterval = core::convertYAML<config::Option<int>>(
                sensorNode[objectsUpdateIntervalTag]);
            auto connectionId = core::convertYAML<config::Option<ConnectionId>>(
                sensorNode[connectionTag]);
            auto executorIt = executors.find(connectionId.value);
            if (executorIt == executors.end())
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Invalid connection id: ")
                        + connectionId.value));
            }
            QString model;
            auto modelNode = sensorNode[modelTag];
            if (modelNode.IsDefined() && !modelNode.IsNull())
            {
                auto modelStr =
                    core::convertYAML<config::Option<std::string>>(modelNode);
                model = QString::fromStdString(modelStr.value);
            }

            auto sensor = std::make_unique<TamerlanSensor>(
                executorIt->second,
                modbusAddress.value,
                sensorUpdateInterval.value,
                objectsUpdateInterval.value,
                model,
                core::JammerAutoDefenseData{
                    controller->jammers(), controller->pin()},
                components.licenseController);

            // TODO: restore sensor -> jammer links
            auto number = core::convertYAML<config::Option<SensorNumber>>(
                sensorNode[numberTag]);
            if (sensorsMap.find(number.value) != sensorsMap.end())
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Duplicate number for sensor: ")
                        + std::to_string(number.value)));
            }
            sensorsMap.insert(std::make_pair(number.value, sensor.get()));
            SensorJammersConnection conn{sensor.get(), {}};
            sensorsJammersConnection.emplace_back(conn);
            controller->appendSensor(std::move(sensor));
        }
    }

    auto jammersNode = description[jammersTag];
    if (jammersNode.IsDefined() && !jammersNode.IsNull()
        && jammersNode.size() > 0)
    {
        auto jammerNodes = jammersNode.as<std::vector<YAML::Node>>();
        for (const auto& jammerNode: jammerNodes)
        {
            auto modbusAddress = core::convertYAML<config::Option<int>>(
                jammerNode[modbusAddressTag]);
            auto updateInterval = core::convertYAML<config::Option<int>>(
                jammerNode[updateIntervalTag]);
            auto connectionId = core::convertYAML<config::Option<ConnectionId>>(
                jammerNode[connectionTag]);
            auto executorIt = executors.find(connectionId.value);
            if (executorIt == executors.end())
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Invalid connection id: ")
                        + connectionId.value));
            }
            auto sensor = core::convertYAML<config::Option<SensorNumber>>(
                jammerNode[sensorTag]);
            auto sensorIt = sensorsMap.find(sensor.value);
            if (sensorIt == sensorsMap.end())
            {
                BOOST_THROW_EXCEPTION(
                    exception::InvalidConfig() << ::exception::ExceptionInfo(
                        std::string("Invalid sensor number: ")
                        + std::to_string(sensor.value)));
            }

            std::vector<TamerlanBandParams> bandParams;
            auto channelNodes =
                jammerNode[channelsTag].as<std::vector<YAML::Node>>();
            for (const auto& channelNode: channelNodes)
            {
                auto number = core::convertYAML<config::Option<int>>(
                    channelNode[numberTag]);
                auto name = core::convertYAML<config::Option<std::string>>(
                    channelNode[nameTag]);
                bandParams.push_back(TamerlanBandParams{
                    .number = number.value,
                    .name = QString::fromStdString(name.value)});
            }
            QString model;
            auto modelNode = jammerNode[modelTag];
            if (modelNode.IsDefined() && !modelNode.IsNull())
            {
                auto modelStr =
                    core::convertYAML<config::Option<std::string>>(modelNode);
                model = QString::fromStdString(modelStr.value);
            }
            std::optional<QString> groupId = std::nullopt;
            auto groupIdNode = jammerNode[groupIdTag];
            if (groupIdNode.IsDefined() && !groupIdNode.IsNull())
            {
                auto groupIdStr =
                    core::convertYAML<config::Option<std::string>>(groupIdNode);
                groupId = QString::fromStdString(groupIdStr.value);
                if (!components.jammerGroupProvider->contains(*groupId))
                {
                    BOOST_THROW_EXCEPTION(
                        exception::InvalidConfig()
                        << ::exception::ExceptionInfo(
                               std::string("Invalid jammer group id: ")
                               + groupId->toStdString()));
                }
            }

            auto distance =
                core::convertYAML<config::Option<int>>(jammerNode[distanceTag]);

            auto jammer = std::make_unique<TamerlanJammer>(
                executorIt->second,
                modbusAddress.value,
                updateInterval.value,
                bandParams,
                distance.value,
                model,
                *sensorIt->second.get(),
                groupId);

            auto connectionIt = std::find_if(
                sensorsJammersConnection.begin(),
                sensorsJammersConnection.end(),
                [sensorPtr = sensorIt->second.get()](const auto& data)
                { return data.sensor.get() == sensorPtr; });
            Ensures(connectionIt != sensorsJammersConnection.end());
            connectionIt->jammers.emplace_back(jammer.get());

            controller->appendJammer(std::move(jammer));
        }
    }
    controller->setSensorsJammersConnection(sensorsJammersConnection);
    return controller;
}

TamerlanController::TamerlanController(const BackendComponents& components) :
  BackendControllerBase(std::make_unique<BackendSignals>(), components)
{
    QObject::connect(
        jammers()->pin(),
        &core::DeviceProviderSignals::registered,
        jammers()->pin(),
        [this](const auto& deviceId) { connectJammer(deviceId); });

    QObject::connect(
        sensors()->pin(),
        &core::DeviceProviderSignals::registered,
        sensors()->pin(),
        [this](const auto& deviceId) { connectSensor(deviceId); });
}

void TamerlanController::setSensorsJammersConnection(
    const std::vector<SensorJammersConnection>& sensorsJammersConnection)
{
    m_sensorsJammersConnection = sensorsJammersConnection;
}

void TamerlanController::connectSensor(const core::DeviceId& deviceId)
{
    auto sensors = this->sensors();
    auto it = sensors->find(deviceId);
    Ensures(it != sensors->end());

    // Looks like dirty hackery
    auto* tamerlanSensor = dynamic_cast<TamerlanSensor*>(&it->properties());
    Ensures(tamerlanSensor);

    auto sensorConnectionIt = std::find_if(
        m_sensorsJammersConnection.begin(),
        m_sensorsJammersConnection.end(),
        [tamerlanSensor](const auto& data)
        { return data.sensor == tamerlanSensor; });
    Ensures(sensorConnectionIt != m_sensorsJammersConnection.end());
    for (auto& jammer: sensorConnectionIt->jammers)
    {
        jammer->setSensorId(deviceId);
    }

    QObject::connect(
        tamerlanSensor,
        &TamerlanSensor::newObject,
        pin(),
        [this,
         deviceId = it->id(),
         tamerlanSensor,
         serial = it->properties().serial()](
            const core::TrackId& trackId, const QJsonObject& attributes)
        {
            core::auditLogger().logAlarm(
                core::AlarmLevel::CRITICAL,
                core::DeviceClass::toString(tamerlanSensor->deviceClass),
                core::DeviceType::toString(tamerlanSensor->type()),
                nodeKey,
                serial);
            emit pin()->trackStarted(
                deviceId,
                trackId,
                {},
                {.level = core::AlarmLevel::CRITICAL, .score = 0},
                core::TargetClass::DRONE,
                attributes);
        });
    QObject::connect(
        tamerlanSensor,
        &TamerlanSensor::objectChanged,
        pin(),
        [this, deviceId = it->id()](
            const core::TrackId& trackId,
            const auto& data,
            const QJsonObject& attributes)
        {
            emit pin()->trackUpdated(
                deviceId,
                trackId,
                {.level = core::AlarmLevel::CRITICAL, .score = 0},
                core::TargetClass::DRONE,
                attributes,
                core::TrackPoints{core::TrackPoint{
                    .coordinate = std::nullopt,
                    .direction = std::nullopt,
                    .timestamp = QDateTime::currentDateTimeUtc(),
                    .rawData = data}});
        });
    QObject::connect(
        tamerlanSensor,
        &TamerlanSensor::objectRemoved,
        pin(),
        [this, deviceId = it->id()](const core::TrackId& trackId)
        { emit pin()->trackFinished(deviceId, trackId); });
}

void TamerlanController::connectJammer(const core::DeviceId& deviceId)
{
    auto jammers = this->jammers();
    auto it = jammers->find(deviceId);
    Ensures(it != jammers->end());

    // Looks like dirty hackery
    auto* tamerlanJammer = dynamic_cast<TamerlanJammer*>(&it->properties());
    Ensures(tamerlanJammer);

    for (auto& sensorConnections: m_sensorsJammersConnection)
    {
        if (std::find(
                sensorConnections.jammers.begin(),
                sensorConnections.jammers.end(),
                tamerlanJammer)
            != sensorConnections.jammers.end())
        {
            sensorConnections.sensor->addJammer(deviceId, tamerlanJammer);
        }
    }
}

} // namespace mpk::dss::backend::tamerlan
