#include "AliaJammer.h"

#include "AliaController.h"

#include "OAServiceGeo_position_mode.h"
#include "dss-common/modbus/byteorderutils.h"
#include "dss-core/JammerTimeoutControl.h"
#include "dss-core/SensorJammerControl.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "log/Log2.h"
#include "qt/MetaEnum.h"
#include "qt/Strings.h"

#include <QUuid>

#include <algorithm>
#include <array>
#include <iterator>
#include <utility>

namespace mpk::dss::backend::alia
{

using namespace mpk::dss::core;

enum CoilValue : quint16
{
    Off = 0x0000,
    On = 0x00FF
};

constexpr auto serialNumber = "_123456789";

AliaJammer::AliaJammer(const Config& config, QObject* parent) :
  core::JammerTimeoutDeviceControl(
      new core::JammerTimeoutControl(*this, &m_bandSignals)),
  QObject(parent),
  m_connectionString{config.connectionString()},
  m_model{config.model},
  m_swVersion{"1.0.0"},
  m_groupId{config.groupId},
  m_updateTimer{std::make_unique<QTimer>()},
  m_bandSignals{JammerBandSignals()},
  m_positionSignals{std::make_unique<DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<DeviceDisabledSignals>()},
  m_stateSignals{std::make_unique<DeviceStateSignals>()},
  m_hwInfoSignals{std::make_unique<DeviceHwInfoSignals>()}
{
    createSuppressionContexts(config);
    initSuppressionContextsByController(config);
    initModbusConnectionStatus();
    connectSignalsToSlots();
    connectControllers(config);

    // TODO: make client request
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    m_position.azimuth = 45.;
    m_position.coordinate = {60., 30., 0.};

    m_workzone.sectors.push_back(
        {.number = 0,
         .distance = config.distance,
         .minAngle = 0.,
         .maxAngle = 360});

    m_hwInfo.values.emplace(DeviceHwInfo::Tags::temperature, "12 ℃");
    m_hwInfo.values.emplace(DeviceHwInfo::Tags::voltage, "48 V");
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    connect(
        m_updateTimer.get(),
        &QTimer::timeout,
        m_updateTimer.get(),
        [this]()
        {
            if (!m_disabled)
            {
                readCoils();
            }
        });
    m_updateTimer->start(config.updateInterval);
}

AliaJammer::~AliaJammer() = default;

auto AliaJammer::serial() const -> Serial
{
    return m_serial;
}

auto AliaJammer::model() const -> Model
{
    return m_model;
}

auto AliaJammer::swVersion() const -> Version
{
    return m_swVersion;
}

auto AliaJammer::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_connectionString + "-" + m_serial;
    }
    return {};
}

DevicePosition AliaJammer::position() const
{
    return m_position;
}

void AliaJammer::setPosition(const core::DevicePosition& position)
{
    if (m_position != position)
    {
        m_position = position;
        emit m_positionSignals->positionChanged();
    }
}

DevicePositionMode AliaJammer::positionMode() const
{
    return DevicePositionMode::ALWAYS_MANUAL;
}

void AliaJammer::setPositionMode(const core::DevicePositionMode& /*mode*/)
{
    // Nothing to do because position mode for Alia is
    // DevicePositionMode::ALWAYS_MANUAL
}

gsl::not_null<const DevicePositionSignals*> AliaJammer::positionSignals() const
{
    return m_positionSignals.get();
}

DeviceWorkzone AliaJammer::workzone() const
{
    return m_workzone;
}

gsl::not_null<const DeviceWorkzoneSignals*> AliaJammer::workzoneSignals() const
{
    return m_workzoneSignals.get();
}

bool AliaJammer::disabled() const
{
    return m_disabled;
}

void AliaJammer::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        m_disabled = disabled;
        m_bandsInitialDataReceived = false;
        emit m_disabledSignals->disabledChanged(m_disabled);
        core::auditLogger().logJammerDisabled(disabled, nodeKey, m_serial);
    }
}

gsl::not_null<const DeviceDisabledSignals*> AliaJammer::disabledSignals() const
{
    return m_disabledSignals.get();
}

DeviceState::Value AliaJammer::state() const
{
    return m_state;
}

void AliaJammer::setState(core::DeviceState::Value state)
{
    if (state != m_state)
    {
        core::auditLogger().logJammerStateChanged(
            m_state, state, nodeKey, m_serial);

        m_state = state;
        emit m_stateSignals->stateChanged(m_state);
    }
}

gsl::not_null<const DeviceStateSignals*> AliaJammer::stateSignals() const
{
    return m_stateSignals.get();
}

core::JammerBandOption AliaJammer::maxBands()
{
    return core::toBandOptionFull(m_bands.begin(), m_bands.end());
}

std::optional<JammerGroupControl::GroupId> AliaJammer::groupId() const
{
    return m_groupId;
}

JammerBands AliaJammer::bands()
{
    return m_bands;
}

bool AliaJammer::setBands(const JammerBandOption& bandsActive)
{
    for (const auto& band: m_bands)
    {
        bool start =
            std::find(bandsActive.begin(), bandsActive.end(), band.label)
            != bandsActive.end();
        if ((start && !band.active) || (!start && band.active))
        {
            QMetaObject::invokeMethod(
                this,
                start ? "startSuppression" : "stopSuppression",
                Q_ARG(mpk::dss::core::JammerLabel, band.label));
        }
    }
    return true;
}

gsl::not_null<const JammerBandSignals*> AliaJammer::bandSignals() const
{
    return &m_bandSignals;
}

DeviceHwInfo AliaJammer::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const DeviceHwInfoSignals*> AliaJammer::hwInfoSignals() const
{
    return m_hwInfoSignals.get();
}

DeviceId AliaJammer::sensorId() const
{
    return {};
}

void AliaJammer::createSuppressionContexts(const Config& config)
{
    ControllerId controllerId{0};
    std::for_each(
        config.items.cbegin(),
        config.items.cend(),
        [this, &controllerId](const auto& item)
        {
            m_modbusClients.emplace_back(std::make_pair(
                controllerId, std::make_unique<mpk::dss::ModbusClient>()));
            for (const auto& coilAddressPair: item.coilAddresses)
            {
                m_suppressionContexts[coilAddressPair.first] =
                    BandSuppressionContext{
                        .deviceId = item.deviceId,
                        .coilAddress = coilAddressPair.second,
                        .modbusClient = m_modbusClients.back().second.get(),
                        .isSuppressionActive = false};
                m_bands.push_back(core::JammerBand{
                    .label = coilAddressPair.first, .active = false});
            }
            controllerId++;
        });
}

void AliaJammer::initSuppressionContextsByController(const Config& config)
{
    auto modbusClientIt = m_modbusClients.begin();
    std::for_each(
        config.items.cbegin(),
        config.items.cend(),
        [this, &modbusClientIt](const auto& item)
        {
            std::for_each(
                item.coilAddresses.cbegin(),
                item.coilAddresses.cend(),
                [this, &modbusClientIt](const auto& coilAddressPair)
                {
                    auto contextIt =
                        m_suppressionContexts.find(coilAddressPair.first);
                    if (contextIt != m_suppressionContexts.end())
                    {
                        m_suppressionContextsByController.insert(
                            {modbusClientIt->first, &(contextIt->second)});
                    }
                });
            modbusClientIt++;
        });
}

void AliaJammer::connectSignalsToSlots()
{
    for (auto& [_, modbusClient]: m_modbusClients)
    {
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusError,
            this,
            &AliaJammer::onModbusError);
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusConnectionError,
            this,
            &AliaJammer::onModbusConnectionError);
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusConnecting,
            this,
            &AliaJammer::onModbusConnecting);
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusConnected,
            this,
            &AliaJammer::onModbusConnected);
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusDisconnected,
            this,
            &AliaJammer::onModbusDisconnected);
        connect(
            modbusClient.get(),
            &mpk::dss::ModbusClient::modbusReply,
            this,
            &AliaJammer::onModbusReply);
    }
}

void AliaJammer::connectControllers(const Config& config)
{
    auto itemsIt = config.items.begin();
    for (auto& [_, client]: m_modbusClients)
    {
        client->connectDevice(itemsIt->connectionParams);
        itemsIt++;
    }
}

void AliaJammer::initModbusConnectionStatus()
{
    for (auto& [controllerId, _]: m_modbusClients)
    {
        m_modbusConnectionStatus.insert({controllerId, Offline});
    }
}

bool AliaJammer::isModbusConnected()
{
    return std::all_of(
        m_modbusConnectionStatus.cbegin(),
        m_modbusConnectionStatus.cend(),
        [](const auto& connPair) { return connPair.second == Online; });
}

void AliaJammer::readCoils()
{
    for (const auto& [controllerId, _]: m_modbusClients)
    {
        auto range =
            m_suppressionContextsByController.equal_range(controllerId);
        for (auto it = range.first; it != range.second; ++it)
        {
            auto* context = it->second;
            auto pointType = QModbusDataUnit::Coils;
            auto coilAddress =
                toByteOrderValue(context->coilAddress, ByteOrder::BigEndian);
            quint16 valueCount = 1;
            auto slaveAddress = context->deviceId;
            auto* client = context->modbusClient;

            client->sendReadRequest(
                pointType, coilAddress, valueCount, slaveAddress, -1);
        }
    }
}

// Examples raw single coils requests
//
// first coil on/off
// 00 00 FF 00 (00 00 00 00)
// 0B 8E 00 00 00 06 01 05 00 00 FF 00

// second coil on/off
// 00 01 FF 00 (00 01 00 00)
// 12 C2 00 00 00 06 01 05 00 01 FF 00

void AliaJammer::writeSingleCoil(const QString& band, quint16 value)
{
    auto& context = m_suppressionContexts.at(band);
    auto slaveAddress = context.deviceId;
    std::array<quint16, 2> rawData = {context.coilAddress, value};
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    QByteArray data(
        reinterpret_cast<const char*>(rawData.data()), sizeof(rawData));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    QModbusRequest request{QModbusPdu::FunctionCode::WriteSingleCoil, data};

    increaseRequestId();
    m_modbusRequestContexts.insert(ModbusRequestContextsStorage::value_type{
        m_requestId, {.band = band, .value = value, .request = request}});

    auto* modbusClient = context.modbusClient;
    modbusClient->sendRawRequest(request, slaveAddress, m_requestId);
}

void AliaJammer::startSuppression(const JammerLabel& band)
{
    if (state() == core::DeviceState::OK)
    {
        startSuppressSingleBand(band);
    }
}

void AliaJammer::stopSuppression(const JammerLabel& band)
{
    if (state() == core::DeviceState::OK)
    {
        stopSuppressSingleBand(band);
    }
}

void AliaJammer::startSuppressSingleBand(const JammerLabel& label)
{
    writeSingleCoil(label, CoilValue::On);
    core::auditLogger().logSuppressionManualRequested(
        core::JammerBand{label, true}, nodeKey, m_serial);
}

void AliaJammer::stopSuppressSingleBand(const JammerLabel& label)
{
    writeSingleCoil(label, CoilValue::Off);
    core::auditLogger().logSuppressionManualRequested(
        core::JammerBand{label, false}, nodeKey, m_serial);
}

void AliaJammer::increaseRequestId()
{
    ++m_requestId;

    if (m_requestId == std::numeric_limits<int>::max())
    {
        m_requestId = 0;
    }
}

std::optional<AliaJammer::ErrorInfo> AliaJammer::validateModbusReply(
    QModbusReply* reply)
{
    if (reply->error() == QModbusDevice::NoError)
    {
        switch (reply->rawResult().functionCode())
        {
            case QModbusPdu::ReadCoils: return validateReadCoilsReply(reply);
            case QModbusPdu::WriteSingleCoil:
                return validateWriteSingleCoilReply(reply);
            default:
                return ErrorInfo(
                    {.message = QString("Unexpected reply: %1")
                                    .arg(reply->rawResult().functionCode()),
                     .code = QModbusDevice::UnknownError});
        }
    }

    return ErrorInfo({.message = reply->errorString(), .code = reply->error()});
}

auto AliaJammer::validateReadCoilsReply(QModbusReply* reply)
    -> std::optional<ErrorInfo>
{
    if (reply->result().isValid() && reply->rawResult().isValid())
    {
        return std::nullopt;
    }

    return ErrorInfo(
        {.message = "Read coils reply not valid",
         .code = QModbusDevice::UnknownError});
}

auto AliaJammer::validateWriteSingleCoilReply(QModbusReply* reply)
    -> std::optional<ErrorInfo>
{
    auto requestId = reply->property("RequestId").toInt();
    auto& request = m_modbusRequestContexts.at(requestId).request;

    if (request.functionCode() == reply->rawResult().functionCode()
        && request.data() == reply->rawResult().data())
    {
        return std::nullopt;
    }

    return ErrorInfo(
        {.message = "Reply invalid due data mismatch to request",
         .code = QModbusDevice::UnknownError});
}

void AliaJammer::onModbusError(const QString& error, int /*requestId*/)
{
    LOG_WARNING << nodeKey << " " << __FUNCTION__
                << " error: " << error.toStdString();
}

void AliaJammer::onModbusConnectionError(const QString& error)
{
    LOG_WARNING << nodeKey << " " << error.toStdString();
}

void AliaJammer::onModbusConnecting(const ConnectionDetails& details)
{
    LOG_DEBUG << nodeKey << " Connecting to device at "
              << details.TcpParams.IPAddress.toStdString() << ":"
              << details.TcpParams.ServicePort;
}

void AliaJammer::onModbusConnected(const ConnectionDetails& details)
{
    LOG_INFO << nodeKey << " Connected to device "
             << details.TcpParams.IPAddress.toStdString() << ":"
             << details.TcpParams.ServicePort;

    auto controllerId = getControllerId(sender());
    if (controllerId)
    {
        m_modbusConnectionStatus.at(*controllerId) = Online;
    }

    if (m_serial.isEmpty())
    {
        m_serial = serialNumber;
    }
    readCoils();
}

void AliaJammer::onModbusDisconnected(const ConnectionDetails& details)
{
    setState(core::DeviceState::UNAVAILABLE);
    LOG_INFO << nodeKey << " No connection to device "
             << details.TcpParams.IPAddress.toStdString() << ":"
             << details.TcpParams.ServicePort;
}

void AliaJammer::onModbusReply(QModbusReply* reply)
{
    auto oError = validateModbusReply(reply);
    if (!oError)
    {
        if (isModbusConnected())
        {
            setState(core::DeviceState::OK);
        }
        switch (reply->rawResult().functionCode())
        {
            case QModbusPdu::ReadCoils: handleReadCoilsReply(reply); break;
            case QModbusPdu::WriteSingleCoil:
                handleWriteSingleCoilReply(reply);
                break;
            default: break;
        }
    }
    else
    {
        auto requestId = reply->property("RequestId").toInt();
        auto requestIt = m_modbusRequestContexts.find(requestId);

        JammerLabel band;
        if (requestIt != m_modbusRequestContexts.end())
        {
            auto& requestContext = requestIt->second;
            band = requestContext.band;
        }
        onOperationError(band, oError.value());
    }
}

void AliaJammer::onSuppressStateChanged(
    const core::JammerLabel& label, bool active)
{
    auto it = std::find_if(
        m_bands.begin(),
        m_bands.end(),
        [label](const auto& band) { return band.label == label; });
    Ensures(it != m_bands.end());

    if (it->active != active)
    {
        it->active = active;
        core::auditLogger().logSuppressionState(*it, nodeKey, m_serial);
        emit m_bandSignals.bandsChanged();
    }
}

void AliaJammer::handleWriteSingleCoilReply(QModbusReply* reply)
{
    LOG_INFO << "handleWriteSingleCoilReply()"; // log readability
    auto requestId = reply->property("RequestId").toInt();
    auto requestIt = m_modbusRequestContexts.find(requestId);
    auto& requestContext = requestIt->second;

    QDataStream dataStream(reply->rawResult().data());
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream.device()->seek(sizeof(quint16)); // skip output address
    quint16 coilValue{0};
    dataStream >> coilValue;

    auto& band = requestContext.band;
    auto& suppressContext = m_suppressionContexts.at(band);

    auto active = (coilValue == CoilValue::On);
    suppressContext.isSuppressionActive = active;
    onSuppressStateChanged(band, active);

    m_modbusRequestContexts.erase(requestIt);
}

void AliaJammer::handleReadCoilsReply(QModbusReply* reply)
{
    auto requestData = reply->property("RequestData").value<QModbusDataUnit>();
    auto controllerId = getControllerId(sender());
    if (controllerId)
    {
        auto coilAddress =
            toByteOrderValue(requestData.startAddress(), ByteOrder::BigEndian);
        auto context = getSuppressionContext(*controllerId, coilAddress);
        if (context)
        {
            auto coilStatus = reply->result().value(0);
            (*context)->isSuppressionActive = static_cast<bool>(coilStatus);
            updateBands();
        }
    }
}

std::optional<AliaJammer::ControllerId> AliaJammer::getControllerId(
    const QObject* clientPtr)
{
    auto clientIt = std::find_if(
        m_modbusClients.begin(),
        m_modbusClients.end(),
        [clientPtr](const auto& controller)
        { return controller.second.get() == clientPtr; });

    if (clientIt != m_modbusClients.end())
    {
        return clientIt->first;
    }

    return std::nullopt;
}

std::optional<AliaJammer::BandSuppressionContext*> AliaJammer::
    getSuppressionContext(ControllerId controllerId, int coilAddress)
{
    auto range = m_suppressionContextsByController.equal_range(controllerId);
    auto contextIt = std::find_if(
        range.first,
        range.second,
        [&coilAddress](const auto& context)
        { return context.second->coilAddress == coilAddress; });

    if (contextIt != range.second)
    {
        return contextIt->second;
    }

    return std::nullopt;
}

void AliaJammer::updateBands()
{
    for (auto& band: m_bands)
    {
        auto it = m_suppressionContexts.find(band.label);
        Ensures(it != m_suppressionContexts.end());
        if (!m_bandsInitialDataReceived
            || band.active != it->second.isSuppressionActive)
        {
            band.active = it->second.isSuppressionActive;
            core::auditLogger().logSuppressionState(band, nodeKey, m_serial);
        }
    }
    m_bandsInitialDataReceived = true;
}

void AliaJammer::onOperationError(
    const JammerLabel& band, const ErrorInfo& error)
{
    setState(core::DeviceState::UNAVAILABLE);

    auto errorInfo2Str = [](const ErrorInfo& error) -> QString
    {
        return "code: " + QString::number(error.code) + ", "
               + "message: " + error.message;
    };

    QString beginPart{" error occurred"};
    beginPart += band.size() != 0 ? " when operate band " + band : "";

    LOG_WARNING << nodeKey << " " << beginPart << " " << errorInfo2Str(error);
}

QString AliaJammer::Config::connectionString() const
{
    QStringList connections;
    for (const auto& item: items)
    {
        if (item.connectionParams.Type == ConnectionType::Tcp)
        {
            connections.append(
                QString("%1:%2:%3")
                    .arg(
                        item.connectionParams.TcpParams.IPAddress,
                        QString::number(
                            item.connectionParams.TcpParams.ServicePort),
                        QString::number(item.deviceId)));
        }
        else
        {
            connections.append(QString("%1:%2").arg(
                item.connectionParams.SerialParams.PortName,
                QString::number(item.deviceId)));
        }
    }
    return connections.join("-");
}

} // namespace mpk::dss::backend::alia
