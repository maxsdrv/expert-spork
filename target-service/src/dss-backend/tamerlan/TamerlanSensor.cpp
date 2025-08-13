#include "TamerlanSensor.h"

#include "DeviceStatus.h"
#include "TamerlanController.h"
#include "TamerlanJammer.h"

#include "modbus/AsyncModbusCommandExecutor.h"
#include "modbus/CommandBatch.h"
#include "modbus/DeviceInfo.h"
#include "modbus/GeoData.h"
#include "modbus/ModbusConnection.h"
#include "modbus/NetworkData.h"
#include "modbus/Protocol.h"

#include "dss-common/sensors/RfdTargetAttributes.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "json/JsonComposer.h"
#include "log/Log2.h"

#include <QTimer>

#include <mutex>

#ifdef OFF
#undef OFF
#endif

using namespace std::chrono_literals;
using namespace mpk::dss::core;

namespace mpk::dss::backend::tamerlan
{

using SensorRawTransmissionData =
    std::array<std::uint16_t, sizeof(TransmissionData) / 2>;

struct SensorRawChannelData
{
    uint16_t status = 0;
    std::array<std::uint16_t, sizeof(ChannelZone) / 2> zone = {};
    std::array<std::uint16_t, sizeof(ChannelParams) / 2> params = {};
    uint16_t objectsCount = 0;
    std::array<SensorRawTransmissionData, proto::maxSensorChannelObjectsCount>
        objects = {};
};

struct SensorRawData
{
    uint16_t status = 0;
    uint16_t modbusAddress = 0;
    std::array<std::uint16_t, sizeof(NetworkData) / 2> networkData = {};
    uint16_t port = 0;
    std::array<std::uint16_t, sizeof(DeviceInfo) / 2> deviceInfo = {};
    std::array<std::uint16_t, sizeof(GeoData) / 2> geoData = {};
    uint16_t protocolVersion = 0;
    uint16_t channelsCount = 0;
    std::array<SensorRawChannelData, proto::maxSensorChannelsCount> channels =
        {};
    std::array<std::uint16_t, sizeof(JammerData) / 2> jammerData = {};
};

TamerlanSensor::TamerlanSensor(
    std::shared_ptr<AsyncModbusCommandExecutor> executor,
    uint16_t modbusAddress,
    int sensorUpdateInterval,
    int objectsUpdateInterval,
    int minDetectionCount,
    const QString& model,
    core::JammerAutoDefenseData autoData,
    gsl::not_null<const core::LicenseController*> licenseController,
    QObject* parent) :
  core::SensorJammerControl(autoData, licenseController),
  QObject(parent),
  m_executor{std::move(executor)},
  m_sensorUpdateInterval(sensorUpdateInterval),
  m_objectsUpdateInterval(objectsUpdateInterval),
  m_minDetectionCount(minDetectionCount),
  m_model(model),
  m_modelPredefined(!model.isEmpty()),
  m_jammerTimeout{10s},
  m_positionSignals{std::make_unique<DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<DeviceStateSignals>()},
  m_hwInfoSignals{std::make_unique<DeviceHwInfoSignals>()},
  m_rawData{std::make_unique<SensorRawData>()},
  m_mainDataUpdateTimer{std::make_unique<QTimer>()},
  m_sensorDataUpdateTimer{std::make_unique<QTimer>()},
  m_objectsDataUpdateTimer{std::make_unique<QTimer>()}
{
    m_executor->addObject(this);

    m_rawData->modbusAddress = modbusAddress;

    connect(
        m_mainDataUpdateTimer.get(),
        &QTimer::timeout,
        m_mainDataUpdateTimer.get(),
        [this]()
        {
            if (!m_mainDataReceived && !m_disabled)
            {
                requestMainData();
            }
        });
    m_mainDataUpdateTimer->start(m_sensorUpdateInterval);

    connect(
        m_sensorDataUpdateTimer.get(),
        &QTimer::timeout,
        m_sensorDataUpdateTimer.get(),
        [this]()
        {
            if (!m_disabled)
            {
                requestSensorData();
            }
        });
    m_sensorDataUpdateTimer->start(m_sensorUpdateInterval);

    connect(
        m_objectsDataUpdateTimer.get(),
        &QTimer::timeout,
        m_objectsDataUpdateTimer.get(),
        [this]()
        {
            if (!m_disabled)
            {
                requestObjectData();
            }
        });
    m_objectsDataUpdateTimer->start(m_objectsUpdateInterval);

    requestMainData();
    requestSensorData();

    unsigned short jammerModeToSet = 0x00; // MANUAL
    m_executor->writeHoldingRegisters(
        QUuid::createUuid(),
        proto::JammerData::address + offsetof(JammerData, mode) / 2,
        1,
        &jammerModeToSet,
        ModbusServerAddress(m_rawData->modbusAddress));
}

TamerlanSensor::~TamerlanSensor() = default;

DeviceType::Value TamerlanSensor::type() const
{
    return DeviceType::RFD;
}

auto TamerlanSensor::serial() const -> Serial
{
    return m_serial;
}

auto TamerlanSensor::model() const -> Model
{
    return m_model;
}

auto TamerlanSensor::swVersion() const -> Version
{
    return m_swVersion;
}

auto TamerlanSensor::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_executor->constConnection()->address()
               + "-" + QString::number(m_rawData->modbusAddress) + "-"
               + m_serial;
    }
    return {};
}

bool TamerlanSensor::disabled() const
{
    return m_disabled;
}

void TamerlanSensor::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        m_disabled = disabled;
        if (m_disabled)
        {
            m_mainDataReceived = false;
        }
        m_executor->setDisabled(this, disabled);
        emit m_disabledSignals->disabledChanged(disabled);
    }
}
gsl::not_null<const core::DeviceDisabledSignals*> TamerlanSensor::
    disabledSignals() const
{
    return m_disabledSignals.get();
}

core::DevicePosition TamerlanSensor::position() const
{
    return m_positionMode == DevicePositionMode::AUTO ? m_autoPosition :
                                                        m_manualPosition;
}

void TamerlanSensor::setPosition(const core::DevicePosition& position)
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

DevicePositionMode TamerlanSensor::positionMode() const
{
    return m_positionMode;
}

void TamerlanSensor::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> TamerlanSensor::
    positionSignals() const
{
    return m_positionSignals.get();
}

core::DeviceWorkzone TamerlanSensor::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> TamerlanSensor::
    workzoneSignals() const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value TamerlanSensor::state() const
{
    return m_state;
}

gsl::not_null<const core::DeviceStateSignals*> TamerlanSensor::stateSignals()
    const
{
    return m_stateSignals.get();
}

core::DeviceHwInfo TamerlanSensor::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> TamerlanSensor::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

std::vector<DeviceId> TamerlanSensor::jammerIds() const
{
    std::vector<DeviceId> result;
    std::transform(
        m_jammers.begin(),
        m_jammers.end(),
        std::back_inserter(result),
        [](const auto& idJammerPair) { return idJammerPair.first; });
    return result;
}

void TamerlanSensor::addJammer(
    const DeviceId& id, gsl::not_null<TamerlanJammer*> jammer)
{
    m_jammers.emplace_back(id, jammer);
}

int TamerlanSensor::channelAddress(int channelIndex)
{
    return proto::firstChannelDataAddress
           + channelIndex * proto::nextSensorChannelDataOffset;
}

void TamerlanSensor::requestMainData()
{
    CommandBatch::Commands commands;

    commands.push_back(
        {Command::READ_HOLDING_REGISTERS,
         proto::NetworkData::address,
         proto::NetworkData::size,
         m_rawData->networkData.data(),
         ModbusServerAddress(m_rawData->modbusAddress)});
    commands.push_back(
        {Command::READ_HOLDING_REGISTERS,
         proto::ServicePort::address,
         proto::ServicePort::size,
         &m_rawData->port,
         ModbusServerAddress(m_rawData->modbusAddress)});
    commands.push_back(
        {Command::READ_INPUT_REGISTERS,
         proto::DeviceInfo::address,
         proto::DeviceInfo::size,
         m_rawData->deviceInfo.data(),
         ModbusServerAddress(m_rawData->modbusAddress)});
    commands.push_back(
        {Command::READ_INPUT_REGISTERS,
         proto::ProtocolVersion::address,
         proto::ProtocolVersion::size,
         &m_rawData->protocolVersion,
         ModbusServerAddress(m_rawData->modbusAddress)});

    if (std::find_if(
            m_commandBatches.begin(),
            m_commandBatches.end(),
            [&commands](const auto& batch)
            { return batch->commands() == commands; })
        == m_commandBatches.end())
    {
        m_commandBatches.push_back(std::make_unique<CommandBatch>(commands));
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::succeeded,
            this,
            [this]()
            {
                setOnline(true);
                updateMainData();
                m_mainDataReceived = true;
            });
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::failed,
            this,
            [this]() { setOnline(false); });
        runNextCommand();
    }
}

void TamerlanSensor::requestSensorData()
{
    CommandBatch::Commands commands;

    commands.push_back(
        {Command::READ_INPUT_REGISTERS,
         proto::DeviceStatus::address,
         proto::DeviceStatus::size,
         &m_rawData->status,
         ModbusServerAddress(m_rawData->modbusAddress)});
    commands.push_back(
        {Command::READ_HOLDING_REGISTERS,
         proto::GeoData::address,
         proto::GeoData::size,
         m_rawData->geoData.data(),
         ModbusServerAddress(m_rawData->modbusAddress)});
    commands.push_back(
        {Command::READ_INPUT_REGISTERS,
         proto::ChannelsCount::address,
         proto::ChannelsCount::size,
         &m_rawData->channelsCount,
         ModbusServerAddress(m_rawData->modbusAddress)});
    for (int index = 0; index < static_cast<int>(m_rawData->channelsCount);
         index++)
    {
        commands.push_back(
            {Command::READ_HOLDING_REGISTERS,
             channelAddress(index) + proto::ChannelZone::relativeAddress,
             proto::ChannelZone::size,
             m_rawData->channels.at(index).zone.data(),
             ModbusServerAddress(m_rawData->modbusAddress)});
        commands.push_back(
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index) + proto::ChannelParams::relativeAddress,
             proto::ChannelParams::size,
             m_rawData->channels.at(index).params.data(),
             ModbusServerAddress(m_rawData->modbusAddress)});
    }
    commands.push_back(
        {Command::READ_HOLDING_REGISTERS,
         proto::JammerData::address,
         proto::JammerData::size,
         m_rawData->jammerData.data(),
         ModbusServerAddress(m_rawData->modbusAddress)});

    if (std::find_if(
            m_commandBatches.begin(),
            m_commandBatches.end(),
            [&commands](const auto& batch)
            { return batch->commands() == commands; })
        == m_commandBatches.end())
    {
        m_commandBatches.push_back(std::make_unique<CommandBatch>(commands));
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::succeeded,
            this,
            [this]()
            {
                setOnline(true);
                updateSensorData();
            });
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::failed,
            this,
            [this]() { setOnline(false); });
        runNextCommand();
    }
}

void TamerlanSensor::requestObjectData()
{
    CommandBatch::Commands commands;

    commands.push_back(
        {Command::READ_INPUT_REGISTERS,
         proto::ChannelsCount::address,
         proto::ChannelsCount::size,
         &m_rawData->channelsCount,
         ModbusServerAddress(m_rawData->modbusAddress)});

    for (int index = 0; index < static_cast<int>(m_rawData->channelsCount);
         index++)
    {
        commands.push_back(
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index) + proto::ChannelStatus::relativeAddress,
             proto::ChannelStatus::size,
             &m_rawData->channels.at(index).status,
             ModbusServerAddress(m_rawData->modbusAddress)});
        commands.push_back(
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index)
                 + proto::ChannelObjectsCount::relativeAddress,
             proto::ChannelObjectsCount::size,
             &m_rawData->channels.at(index).objectsCount,
             ModbusServerAddress(m_rawData->modbusAddress)});

        for (int objIndex = 0; objIndex < static_cast<int>(
                                   m_rawData->channels.at(index).objectsCount);
             objIndex++)
        {
            commands.push_back(
                {Command::READ_INPUT_REGISTERS,
                 channelAddress(index) + proto::Alarm::relativeAddress
                     + static_cast<int>(
                         objIndex
                         * m_rawData->channels.at(index)
                               .objects.at(objIndex)
                               .size()),
                 proto::Alarm::size,
                 m_rawData->channels.at(index).objects.at(objIndex).data(),
                 ModbusServerAddress(m_rawData->modbusAddress)});
        }
    }

    if (std::find_if(
            m_commandBatches.begin(),
            m_commandBatches.end(),
            [&commands](const auto& batch)
            { return batch->commands() == commands; })
        == m_commandBatches.end())
    {
        m_commandBatches.push_back(std::make_unique<CommandBatch>(commands));
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::succeeded,
            this,
            [this]()
            {
                setOnline(true);
                updateObjectsData();
            });
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::failed,
            this,
            [this]() { setOnline(false); });
        runNextCommand();
    }
}

void TamerlanSensor::updateMainData()
{
    // TODO: this is crunches
    auto lock = std::scoped_lock{*m_executor};

    // TODO: this is unused for now
    /*m_networkData = NetworkData::fromRegs(
            m_rawData.networkData.begin(), m_rawData.networkData.end());*/
    auto deviceInfo = DeviceInfo::fromRegs(
        m_rawData->deviceInfo.begin(), m_rawData->deviceInfo.end());
    auto swVersion = std::string{};
    auto stream = std::ostringstream{swVersion};
    DeviceInfo::streamSwVersion(stream, deviceInfo);

    if (!m_modelPredefined)
    {
        m_model = QString::fromUtf8(deviceInfo.model.data());
    }
    m_serial = QString::fromUtf8(deviceInfo.serial.data());
}

void TamerlanSensor::updateSensorData()
{
    // TODO: this is crunches
    auto lock = std::scoped_lock{*m_executor};

    auto status = static_cast<DeviceStatus>(m_rawData->status);
    auto state = disabled() ? core::DeviceState::OFF :
                              (status == DeviceStatus::NORMAL_MODE ?
                                   core::DeviceState::OK :
                                   (status == DeviceStatus::SERVICE_MODE ?
                                        core::DeviceState::UNAVAILABLE :
                                        core::DeviceState::FAILURE));
    setState(state);

    auto geoData =
        GeoData::fromRegs(m_rawData->geoData.begin(), m_rawData->geoData.end());
    auto position = core::DevicePosition{
        .azimuth = geoData.orientation,
        .coordinate = QGeoCoordinate(geoData.latitude, geoData.longitude)};
    auto oldAutoPosition = m_autoPosition;
    m_autoPosition = position;
    if (m_positionMode == DevicePositionMode::AUTO
        && oldAutoPosition != position)
    {
        emit m_positionSignals->positionChanged();
    }

    if (m_rawData->channelsCount > proto::maxSensorChannelsCount)
    {
        LOG_WARNING << "Incorrect sensor channels count: "
                    << m_rawData->channelsCount;
        m_rawData->channelsCount = 0;
    }
    if (m_channels.size() != m_rawData->channelsCount)
    {
        m_channels.resize(m_rawData->channelsCount);
        m_workzone.sectors.resize(m_rawData->channelsCount);
    }

    auto jammerData = JammerData::fromRegs(
        m_rawData->jammerData.begin(), m_rawData->jammerData.end());

    for (int i = 0; i < static_cast<int>(jammerData.addresses.size()); i++)
    {
        if (jammerData.addresses.at(i) != m_jammerAddresses.at(i)
            && i < static_cast<int>(m_jammers.size()))
        {
            m_jammers[i].second->changeModbusAddress(
                jammerData.addresses.at(i));
        }
    }
    m_jammerAddresses = jammerData.addresses;

    core::DeviceWorkzone workzone;
    workzone.sectors.resize(m_channels.size());
    for (int index = 0; index < static_cast<int>(m_channels.size()); index++)
    {
        auto channelZone = ChannelZone::fromRegs(
            m_rawData->channels.at(index).zone.begin(),
            m_rawData->channels.at(index).zone.end());
        m_channels[index].zone = channelZone;
        auto sector = core::DeviceWorkzoneSector{
            .number = index,
            .distance = channelZone.distance,
            .minAngle = channelZone.direction - channelZone.workAngle / 2,
            .maxAngle = channelZone.direction + channelZone.workAngle / 2};
        workzone.sectors[index] = sector;

        m_channels[index].params = ChannelParams::fromRegs(
            m_rawData->channels.at(index).params.begin(),
            m_rawData->channels.at(index).params.end());
    }
    if (workzone != m_workzone)
    {
        m_workzone = workzone;
        emit m_workzoneSignals->workzoneChanged(m_workzone);
    }
}

void TamerlanSensor::updateObjectsData()
{
    // TODO: this is crunches
    auto lock = std::scoped_lock{*m_executor};

    if (m_rawData->channelsCount > proto::maxSensorChannelsCount)
    {
        LOG_WARNING << "Incorrect sensor channels count: "
                    << m_rawData->channelsCount;
        m_rawData->channelsCount = 0;
    }
    if (m_channels.size() != m_rawData->channelsCount)
    {
        m_channels.resize(m_rawData->channelsCount);
    }

    for (int index = 0; index < static_cast<int>(m_channels.size()); index++)
    {
        m_channels[index].status =
            static_cast<ChannelStatus>(m_rawData->channels.at(index).status);
        std::vector<TransmissionData> transmissions;
        for (int i = 0;
             i < static_cast<int>(m_rawData->channels.at(index).objectsCount);
             i++)
        {
            auto transmission = TransmissionData::fromRegs(
                m_rawData->channels.at(index).objects.at(i).begin(),
                m_rawData->channels.at(index).objects.at(i).end());
            if (transmission.detectionLevel >= m_minDetectionCount)
            {
                transmissions.push_back(transmission);
            }
            else
            {
                LOG_DEBUG
                    << "Object not processed due to low detection level: channelIndex "
                    << index << " transmission.id " << transmission.id
                    << " detectionLevel " << transmission.detectionLevel;
            }
        }
        updateObjects(index, transmissions);
    }
}

void TamerlanSensor::updateObjects(
    int channelIndex, const std::vector<TransmissionData>& transmissions)
{
    for (const auto& transmission: m_channels[channelIndex].transmissions)
    {
        auto& trackData = m_channels[channelIndex].trackIds.at(
            static_cast<int>(transmission.id));
        auto it = std::find_if(
            transmissions.begin(),
            transmissions.end(),
            [id = transmission.id](const auto& value)
            { return id == value.id; });
        if (it == transmissions.end())
        {
            emit objectRemoved(trackData.trackId);
        }
        else
        {
            if (it->timestamp != transmission.timestamp)
            {
                auto attributes = getTargetAttributes(transmission);
                emit objectChanged(
                    trackData.trackId,
                    json::toValue(transmission).toObject(),
                    json::toValue(attributes).toObject(),
                    transmission.detectionLevel);
                LOG_DEBUG << "Object updated: channelIndex " << channelIndex
                          << " transmission.id " << transmission.id
                          << " detectionLevel " << transmission.detectionLevel;
            }
        }
    }
    for (const auto& transmission: transmissions)
    {
        auto it = std::find_if(
            m_channels[channelIndex].transmissions.begin(),
            m_channels[channelIndex].transmissions.end(),
            [id = transmission.id](const auto& value)
            { return id == value.id; });
        if (it == m_channels[channelIndex].transmissions.end())
        {
            auto trackId = core::TrackId::generate();
            auto trackData = TrackData{.trackId = trackId};
            m_channels[channelIndex]
                .trackIds[static_cast<int>(transmission.id)] = trackData;
            auto attributes = getTargetAttributes(transmission);
            emit newObject(trackId, json::toValue(attributes).toObject());
            emit objectChanged(
                trackId,
                json::toValue(transmission).toObject(),
                json::toValue(attributes).toObject(),
                transmission.detectionLevel);
            LOG_DEBUG << "New object: channelIndex " << channelIndex
                      << " transmission.id " << transmission.id
                      << " transmission.detectionLevel "
                      << transmission.detectionLevel;
        }
    }
    m_channels[channelIndex].transmissions = transmissions;
}

void TamerlanSensor::setOnline(bool online)
{
    auto newState = disabled() ? core::DeviceState::OFF :
                                 (online ? core::DeviceState::OK :
                                           core::DeviceState::UNAVAILABLE);
    setState(newState);
}

void TamerlanSensor::setState(core::DeviceState::Value state)
{
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

void TamerlanSensor::runNextCommand()
{
    if (!m_commandBatches.empty() && !m_commandBatches.front()->running())
    {
        connect(
            m_commandBatches.front().get(),
            &CommandBatch::finished,
            this,
            [this]()
            {
                m_commandBatches.pop_front();
                runNextCommand();
            });
        m_commandBatches.front()->execute(m_executor.get());
    }
}

sensors::RfdTargetAttributes TamerlanSensor::getTargetAttributes(
    const TransmissionData& transmission)
{
    return sensors::RfdTargetAttributes{
        .digital = (std::string(transmission.name.data()) != "FPV"),
        .frequencies = {transmission.beginFreq, transmission.endFreq},
        .description = transmission.name.data()};
}

} // namespace mpk::dss::backend::tamerlan
