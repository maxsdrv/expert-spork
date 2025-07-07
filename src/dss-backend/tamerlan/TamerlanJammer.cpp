#include "TamerlanJammer.h"

#include "DeviceStatus.h"
#include "TamerlanController.h"

#include "modbus/AsyncModbusCommandExecutor.h"
#include "modbus/Common.h"
#include "modbus/DeviceInfo.h"
#include "modbus/GeoData.h"
#include "modbus/ModbusConnection.h"
#include "modbus/Protocol.h"

#include "dss-backend/tamerlan/modbus/Common.h"
#include "dss-core/JammerTimeoutControl.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "log/Log2.h"
#include "qttypes/QStringConverters.h"

#include <QTimer>
#include <QUuid>

#include <array>
#include <iosfwd>
#include <mutex>

#ifdef OFF
#undef OFF
#endif

using namespace std::chrono_literals;
using namespace mpk::dss::core;

namespace mpk::dss::backend::tamerlan
{

struct JammerRawChannelData
{
    uint16_t status = 0;
    uint8_t emission = 0;
    std::array<std::uint16_t, proto::JammerChannelName::size> name = {};
    std::array<std::uint16_t, 2> voltage = {};
    std::array<std::uint16_t, 2> current = {};

    [[nodiscard]] QString nameString() const
    {
        auto nameArray = name;
        std::array<char, proto::JammerChannelName::size> nameChar = {};
        regsToCharArray(nameChar.data(), nameChar.size(), nameArray.begin());
        return QString::fromUtf8(nameChar.data());
    }
};

struct JammerRawData
{
    uint16_t status = 0;
    uint16_t modbusAddress = 0;
    std::array<std::uint16_t, sizeof(DeviceInfo) / 2> deviceInfo = {};
    std::array<std::uint16_t, sizeof(GeoData) / 2> geoData = {};
    uint16_t protocolVersion = 0;
    std::array<std::uint16_t, 2> deviceMaxTemperature = {};
    uint16_t channelsCount = 0;
    std::array<JammerRawChannelData, proto::maxJammerChannelsCount> channels =
        {};
};

/* TODO: This code is not used
bool TamerlanJammer::isSuppressionActive(const Band& band)
{
    if (m_bandIndexes.contains(band) && state() == core::DeviceState::OK)
    {
        auto channelAddress =
            proto::firstChannelDataAddress
            + m_bandIndexes.value(band) * proto::nextJammerChannelDataOffset;
        uint8_t emissionStatus = 0;
        m_connection->readCoils(
            channelAddress + proto::JammerControl::relativeAddress,
            proto::JammerControl::size,
            &emissionStatus);
        return emissionStatus != 0U;
    }
    return false;
}
*/

TamerlanJammer::TamerlanJammer(
    std::shared_ptr<AsyncModbusCommandExecutor> executor,
    uint16_t modbusAddress,
    int updateInterval,
    const std::vector<TamerlanBandParams>& bandParams,
    int distance,
    const QString& model,
    SensorJammerControl& sensor,
    const std::optional<GroupId>& groupId,
    QObject* parent) :
  core::JammerTimeoutDeviceControl(
      new core::JammerTimeoutControl(*this, &m_bandSignals)),
  QObject(parent),
  m_executor{std::move(executor)},
  m_updateInterval(updateInterval),
  m_model{model},
  m_modelPredefined{!model.isEmpty()},
  m_groupId(groupId),
  m_bandSignals{JammerBandSignals()},
  m_positionSignals{std::make_unique<DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<DeviceStateSignals>()},
  m_hwInfoSignals{std::make_unique<DeviceHwInfoSignals>()},
  m_rawData{std::make_unique<JammerRawData>()},
  m_mainDataUpdateTimer{std::make_unique<QTimer>()},
  m_updateTimer{std::make_unique<QTimer>()},
  m_sensor(sensor)
{
    m_executor->addObject(this);

    constexpr double circleAngle = 360.0;
    m_workzone.sectors.emplace_back(core::DeviceWorkzoneSector{
        0, static_cast<double>(distance), 0, circleAngle});

    m_hwInfo.values.emplace(DeviceHwInfo::Tags::temperature, "Unavailable");
    m_hwInfo.values.emplace(DeviceHwInfo::Tags::voltage, "Unavailable");
    m_hwInfo.values.emplace(DeviceHwInfo::Tags::current, "Unavailable");

    for (const auto& params: bandParams)
    {
        m_bandData.emplace_back(
            BandData{.active = false, .params = params, .emissionToSet = 0});
    }

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
    m_mainDataUpdateTimer->start(updateInterval);

    connect(
        m_updateTimer.get(),
        &QTimer::timeout,
        m_updateTimer.get(),
        [this]()
        {
            if (!m_disabled)
            {
                requestData();
            }
        });
    m_updateTimer->start(updateInterval);

    requestMainData();
}

TamerlanJammer::~TamerlanJammer() = default;

auto TamerlanJammer::serial() const -> Serial
{
    return m_serial;
}

auto TamerlanJammer::model() const -> Model
{
    return m_model;
}

auto TamerlanJammer::swVersion() const -> Version
{
    return m_swVersion;
}

auto TamerlanJammer::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_executor->constConnection()->address()
               + "-" + QString::number(m_rawData->modbusAddress) + "-"
               + m_serial;
    }
    return {};
}

DevicePosition TamerlanJammer::position() const
{
    return m_positionMode == DevicePositionMode::AUTO ? m_autoPosition :
                                                        m_manualPosition;
}

void TamerlanJammer::setPosition(const core::DevicePosition& position)
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

DevicePositionMode TamerlanJammer::positionMode() const
{
    return m_positionMode;
}

void TamerlanJammer::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const DevicePositionSignals*> TamerlanJammer::positionSignals()
    const
{
    return m_positionSignals.get();
}

DeviceWorkzone TamerlanJammer::workzone() const
{
    return m_workzone;
}

gsl::not_null<const DeviceWorkzoneSignals*> TamerlanJammer::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

bool TamerlanJammer::disabled() const
{
    return m_disabled;
}

void TamerlanJammer::setDisabled(bool disabled)
{
    if (m_disabled != disabled)
    {
        m_disabled = disabled;
        if (m_disabled)
        {
            m_mainDataReceived = false;
            m_bandsInitialDataReceived = false;
        }
        m_executor->setDisabled(this, disabled);
        emit m_disabledSignals->disabledChanged(m_disabled);
        core::auditLogger().logJammerDisabled(disabled, nodeKey, m_serial);
    }
}

gsl::not_null<const DeviceDisabledSignals*> TamerlanJammer::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

DeviceState::Value TamerlanJammer::state() const
{
    return m_state;
}

gsl::not_null<const DeviceStateSignals*> TamerlanJammer::stateSignals() const
{
    return m_stateSignals.get();
}

core::JammerBandOption TamerlanJammer::maxBands()
{
    JammerBandOption bandOpt;
    std::transform(
        m_bandData.begin(),
        m_bandData.end(),
        std::inserter(bandOpt, bandOpt.begin()),
        [](const BandData& b) { return b.params.name; });
    return bandOpt;
}

std::optional<JammerGroupControl::GroupId> TamerlanJammer::groupId() const
{
    return m_groupId;
}

JammerBands TamerlanJammer::bands()
{
    core::JammerBands bands;
    for (auto i = 0U;
         i < std::min(
             static_cast<size_t>(m_rawData->channelsCount), m_bandData.size());
         i++)
    {
        const auto& bandData = m_bandData.at(i);
        bands.emplace_back(core::JammerBand{
            .label = bandData.params.name, .active = bandData.active});
    }
    return bands;
}

bool TamerlanJammer::setBands(const core::JammerBandOption& bandsActive)
{
    for (const auto& band: m_bandData)
    {
        auto it =
            std::find(bandsActive.begin(), bandsActive.end(), band.params.name);

        setSuppression(band.params.name, it != bandsActive.end());
    }

    return true;
}

gsl::not_null<const JammerBandSignals*> TamerlanJammer::bandSignals() const
{
    return &m_bandSignals;
}

DeviceHwInfo TamerlanJammer::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const DeviceHwInfoSignals*> TamerlanJammer::hwInfoSignals() const
{
    return m_hwInfoSignals.get();
}

DeviceId TamerlanJammer::sensorId() const
{
    return m_sensorId;
}

void TamerlanJammer::setSensorId(const core::DeviceId& sensorId)
{
    m_sensorId = sensorId;
}

void TamerlanJammer::changeModbusAddress(uint16_t value)
{
    m_rawData->modbusAddress = value;
}

void TamerlanJammer::setSuppression(bool value, bool manual)
{
    m_emissionToSet = value ? 0x01 : 0x00;
    m_executor->writeCoils(
        QUuid::createUuid(),
        proto::JammerAllChannelsControl::address,
        proto::JammerAllChannelsControl::size,
        &m_emissionToSet,
        ModbusServerAddress(m_rawData->modbusAddress));

    for (auto& band: m_bandData)
    {
        if (manual)
        {
            core::auditLogger().logSuppressionManualRequested(
                core::JammerBand{band.params.name, value}, nodeKey, m_serial);
        }
        else
        {
            core::auditLogger().logSuppressionAutoRequested(
                core::JammerBand{band.params.name, value}, nodeKey, m_serial);
        }
    }
}

void TamerlanJammer::setSuppression(const JammerLabel& label, bool value)
{
    auto it = std::find_if(
        m_bandData.begin(),
        m_bandData.end(),
        [label](const auto& band) { return band.params.name == label; });
    Ensures(it != m_bandData.end());

    auto index = it->params.number - 1;
    if (index < m_rawData->channelsCount)
    {
        it->emissionToSet = value ? 0x01 : 0x00;
        m_executor->writeCoils(
            QUuid::createUuid(),
            channelAddress(static_cast<int>(index))
                + proto::JammerChannelControl::relativeAddress,
            proto::JammerChannelControl::size,
            &(it->emissionToSet),
            ModbusServerAddress(m_rawData->modbusAddress));

        core::auditLogger().logSuppressionManualRequested(
            core::JammerBand{label, value}, nodeKey, m_serial);
    }
}

int TamerlanJammer::channelAddress(int channelIndex)
{
    return proto::firstChannelDataAddress
           + channelIndex * proto::nextJammerChannelDataOffset;
}

void TamerlanJammer::requestMainData()
{
    CommandBatch::Commands commands;

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

void TamerlanJammer::requestData()
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
         proto::DeviceMaxTemperature::address,
         proto::DeviceMaxTemperature::size,
         m_rawData->deviceMaxTemperature.data(),
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
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index) + proto::ChannelStatus::relativeAddress,
             proto::ChannelStatus::size,
             &m_rawData->channels.at(index).status,
             ModbusServerAddress(m_rawData->modbusAddress)});
        commands.push_back(
            {Command::READ_COILS,
             channelAddress(index)
                 + proto::JammerChannelControl::relativeAddress,
             proto::JammerChannelControl::size,
             &m_rawData->channels.at(index).emission,
             ModbusServerAddress(m_rawData->modbusAddress)});
        if (m_rawData->channels.at(index).nameString().isEmpty())
        {
            commands.push_back(
                {Command::READ_INPUT_REGISTERS,
                 channelAddress(index)
                     + proto::JammerChannelName::relativeAddress,
                 proto::JammerChannelName::size,
                 m_rawData->channels.at(index).name.data(),
                 ModbusServerAddress(m_rawData->modbusAddress)});
        }
        commands.push_back(
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index)
                 + proto::JammerChannelVoltage::relativeAddress,
             proto::JammerChannelVoltage::size,
             m_rawData->channels.at(index).voltage.data(),
             ModbusServerAddress(m_rawData->modbusAddress)});
        commands.push_back(
            {Command::READ_INPUT_REGISTERS,
             channelAddress(index)
                 + proto::JammerChannelCurrent::relativeAddress,
             proto::JammerChannelCurrent::size,
             m_rawData->channels.at(index).current.data(),
             ModbusServerAddress(m_rawData->modbusAddress)});
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
            [this]() { updateData(); });
        connect(
            m_commandBatches.back().get(),
            &CommandBatch::failed,
            this,
            [this]() { setOnline(false); });
        runNextCommand();
    }
}

void TamerlanJammer::updateMainData()
{
    // TODO: crunches
    auto lock = std::scoped_lock{*m_executor};

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

void TamerlanJammer::updateData()
{
    // TODO: crunches
    auto lock = std::scoped_lock{*m_executor};

    if (m_rawData->channelsCount > proto::maxJammerChannelsCount)
    {
        LOG_WARNING << "Incorrect jammer channels count: "
                    << m_rawData->channelsCount;
        m_rawData->channelsCount = 0;
    }

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

    float deviceMaxTemperature = 0;
    auto* deviceMaxTemperatureIt = m_rawData->deviceMaxTemperature.begin();
    std::tie(deviceMaxTemperature, deviceMaxTemperatureIt) =
        regsToFloat(deviceMaxTemperatureIt);
    m_hwInfo.values[DeviceHwInfo::Tags::temperature] =
        QString("%1 ℃").arg(QString::number(deviceMaxTemperature, 'f', 2));

    QStringList voltageList;
    QStringList currentList;
    bool bandsChanged = false;
    for (int index = 0; index < static_cast<int>(m_rawData->channelsCount);
         index++)
    {
        // TODO: Add output of band names from device if needed
        // auto nameStr = m_rawData->channels.at(index).nameString();

        auto it = std::find_if(
            m_bandData.begin(),
            m_bandData.end(),
            [index](const auto& band)
            { return band.params.number == index + 1; });
        if (it != m_bandData.end())
        {
            auto active = (m_rawData->channels.at(index).emission != 0U);
            if (it->active != active)
            {
                it->active = active;
                if (m_bandsInitialDataReceived)
                {
                    core::auditLogger().logSuppressionState(
                        core::JammerBand{
                            .label = it->params.name, .active = it->active},
                        nodeKey,
                        m_serial);
                }
                bandsChanged = true;
            }

            float voltage = 0;
            auto* voltageIt = m_rawData->channels.at(index).voltage.begin();
            std::tie(voltage, voltageIt) = regsToFloat(voltageIt);
            voltageList.append(
                QString("%1 mV").arg(QString::number(voltage, 'f', 2)));

            float current = 0;
            auto* currentIt = m_rawData->channels.at(index).current.begin();
            std::tie(current, currentIt) = regsToFloat(currentIt);
            currentList.append(
                QString("%1 mA").arg(QString::number(current, 'f', 2)));
        }
    }
    m_hwInfo.values[DeviceHwInfo::Tags::voltage] = voltageList.join("; ");
    m_hwInfo.values[DeviceHwInfo::Tags::current] = currentList.join("; ");

    if (!m_bandsInitialDataReceived)
    {
        for (const auto& band: m_bandData)
        {
            core::auditLogger().logSuppressionState(
                core::JammerBand{
                    .label = band.params.name, .active = band.active},
                nodeKey,
                m_serial);
        }
        m_bandsInitialDataReceived = true;
    }
    if (bandsChanged)
    {
        emit m_bandSignals.bandsChanged();
    }
}

void TamerlanJammer::setOnline(bool online)
{
    auto newState = disabled() ? core::DeviceState::OFF :
                                 (online ? core::DeviceState::OK :
                                           core::DeviceState::UNAVAILABLE);
    setState(newState);
}

void TamerlanJammer::setState(core::DeviceState::Value state)
{
    if (state != m_state)
    {
        core::auditLogger().logJammerStateChanged(
            m_state, state, nodeKey, m_serial);

        m_state = state;
        emit m_stateSignals->stateChanged(m_state);
    }
}

void TamerlanJammer::runNextCommand()
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

} // namespace mpk::dss::backend::tamerlan
