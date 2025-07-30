#include "RadarIqConnection.h"

#include "dss-backend/BackendSignals.h"
#include "dss-common/api/Helpers.h"
#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/HttpTags.h"
#include "dss-common/config/Option.h"
#include "dss-core/DeviceId.h"
#include "dss-core/SensorDevice.h"
#include "dss-core/YamlConvertHelper.h"
#include "httpws/ws/WebSocketClient.h"

#include <QNetworkReply>
#include <QUuid>

namespace
{

enum class MessageTag
{
    Tracks,
    RemoveTracks,
    RadarPosition,
    RadarWorkzone,
    RadarTelemetry,
};

struct Message
{
    QString type;
    QString field;
    std::optional<std::vector<std::pair<QString, QString>>> tags;
};

const std::map<MessageTag, Message> messageTable{
    {MessageTag::Tracks,
     {Message{.type = "tracks", .field = "tracks", .tags{}}}},
    {MessageTag::RemoveTracks,
     {Message{.type = "removetracks", .field = "tracks", .tags{}}}},
    {MessageTag::RadarPosition,
     {Message{.type = "radar/position_data", .field = "data", .tags{}}}},
    {MessageTag::RadarWorkzone,
     {Message{.type = "radar/workzone_data", .field = "data", .tags{}}}},
    {MessageTag::RadarTelemetry,
     {Message{
         .type = "radar/telemetry_parameter_data",
         .field = "tag",
         .tags =
             std::vector<std::pair<QString, QString>>{
                 {"temperature", "data"}, {"mode", "data"}}}}},
};

constexpr auto temperatureTag = "temperature";
constexpr auto modeTag = "mode";

} // namespace

namespace mpk::dss::backend::radariq
{

using namespace std::chrono_literals;

RadarIqConnection::RadarIqConnection(
    const config::ConnectionSettings& httpConnectionSettings,
    const config::ConnectionSettings& wsConnectionSettings,
    const std::chrono::milliseconds& updateIntervalSettings,
    const std::chrono::milliseconds& timeoutSettings,
    const config::AuthSettings& authSettings,
    QObject* parent) :
  QObject(parent),
  m_updateInterval{updateIntervalSettings},
  m_wsConnection{std::make_unique<rest::HttpWsConnection>(
      httpConnectionSettings, wsConnectionSettings, "", this)},
  m_deviceApiClient{
      std::make_unique<OARadarIqDeviceApi>(timeoutSettings.count())},
  m_radarApiClient{
      std::make_unique<OARadarIqRadarApi>(timeoutSettings.count())},
  m_authClient{std::make_unique<RadarIqAuthClient>(
      httpConnectionSettings, authSettings, this)}
{
    auto apiUrl = "http://" + httpConnectionSettings.host + ":"
                  + QString::number(httpConnectionSettings.port) + "/api/v1";
    m_deviceApiClient->setNewServerForAllOperations(apiUrl);
    m_radarApiClient->setNewServerForAllOperations(apiUrl);

    m_authClient->authenticate();

    registerHandlers();
}

void RadarIqConnection::registerHandlers()
{
    for (const auto& msgPair: messageTable)
    {
        const auto messageTag = msgPair.first;
        const auto& messageType = msgPair.second;

        m_wsConnection->webSocketClient().registerHandler(
            messageType.type,
            [this, messageTag](const QJsonObject& json)
            {
                switch (messageTag)
                {
                    case MessageTag::Tracks: handleTracksMessage(json); break;
                    case MessageTag::RemoveTracks:
                        handleTracksRemoveMessage(json);
                        break;
                    case MessageTag::RadarPosition:
                        handlePositionMessage(json);
                        break;
                    case MessageTag::RadarWorkzone:
                        handleWorkzoneMessage(json);
                        break;
                    case MessageTag::RadarTelemetry:
                        handleTelemetryMessage(json);
                        break;
                }
            });
    }

    connect(
        &m_httpTimer, &QTimer::timeout, this, &RadarIqConnection::updateState);

    connect(
        this,
        &RadarIqConnection::updateDisabled,
        this,
        &RadarIqConnection::toggleUpdateDisabled);

    connect(
        m_authClient.get(),
        &RadarIqAuthClient::authenticated,
        this,
        &RadarIqConnection::handleAuthSuccess);
    connect(
        m_authClient.get(),
        &RadarIqAuthClient::authenticationFailed,
        this,
        &RadarIqConnection::handleAuthFailure);

    connect(
        m_deviceApiClient.get(),
        &OARadarIqDeviceApi::getUnitInfoListSignal,
        this,
        &RadarIqConnection::parseUnitInfoList);
    connect(
        m_deviceApiClient.get(),
        &OARadarIqDeviceApi::getUnitInfoListSignalError,
        this,
        [this](const auto&, const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "unit info list",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });
    connect(
        m_deviceApiClient.get(),
        &OARadarIqDeviceApi::getServiceSettingsSignal,
        this,
        &RadarIqConnection::parseServiceSettings);
    connect(
        m_deviceApiClient.get(),
        &OARadarIqDeviceApi::getServiceSettingsSignalError,
        this,
        [this](const auto&, const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "service settings",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });

    connect(
        m_radarApiClient.get(),
        &OARadarIqRadarApi::getRadarTelemetrySignal,
        this,
        &RadarIqConnection::parseRadarTelemetry);
    connect(
        m_radarApiClient.get(),
        &OARadarIqRadarApi::getRadarTelemetrySignalError,
        this,
        [this](const auto&, const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "radar telemetry",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });
}

void RadarIqConnection::setDisabled(bool disabled)
{
    LOG_DEBUG << "set connection disabled: " << disabled;
    m_disabled = disabled;
    emit updateDisabled();
}

void RadarIqConnection::toggleUpdateDisabled()
{
    LOG_DEBUG << "toggle update disabled: " << m_disabled;
    m_disabled ? m_wsConnection->stop() : m_wsConnection->start();
    m_disabled ? m_httpTimer.stop() : m_httpTimer.start();
}

void RadarIqConnection::handleAuthSuccess(
    const QString& accessToken, bool initial)
{
    LOG_DEBUG << "Authenticated";
    m_deviceApiClient->setBearerToken(accessToken);
    m_radarApiClient->setBearerToken(accessToken);

    if (initial)
    {
        m_httpTimer.setInterval(m_updateInterval);
        emit updateDisabled();

        emit updateStarted(
            /*connected=*/true,
            /*faultless=*/true);
        getRadarIqData(true);
    }
    m_httpTimer.setSingleShot(false);
}

void RadarIqConnection::handleAuthFailure(std::chrono::milliseconds retryDelay)
{
    m_httpTimer.setSingleShot(true);
    m_httpTimer.start(retryDelay);

    emit updateStarted(
        /*connected=*/false,
        /*faultless=*/true);
}

void RadarIqConnection::updateState()
{
    LOG_TRACE << "timer";
    emit updateStarted(isConnectionOk(), isFaultless());
    if (!m_authClient->ensureAuthenticated())
    {
        return;
    }
    getRadarIqData();
}

void RadarIqConnection::getRadarIqData(bool initial)
{
    m_httpTimeout = false;
    m_httpError = false;
    if (initial)
    {
        m_deviceApiClient->getServiceSettings();
        m_radarApiClient->getRadarTelemetry();
    }
    m_deviceApiClient->getUnitInfoList();
}

bool RadarIqConnection::isConnectionOk() const
{
    return !m_httpTimeout
           && m_wsConnection->state() == mpk::rest::ConnectionState::CONNECTED;
}

bool RadarIqConnection::isFaultless() const
{
    return !m_httpError
           && m_wsConnection->state() == mpk::rest::ConnectionState::CONNECTED;
}

std::pair<bool, bool> RadarIqConnection::unitIdToDevice(QString unitId) const
{
    if (unitId == m_radarUnitId)
    {
        return std::make_pair(true, true);
    }
    if (unitId == m_cameraUnitId)
    {
        return std::make_pair(true, false);
    }

    LOG_WARNING << "Radar IQ: Unknown unit id: " << unitId;
    return std::make_pair(false, false);
}

void RadarIqConnection::parseUnitInfoList(const OARadarIqUnit_info_list& content)
{
    LOG_TRACE << "unit info list: " << content.asJsonObject();
    if (!content.isValid())
    {
        LOG_WARNING << "unit info list data is not valid";
        return;
    }
    const auto units = content.getInfo();
    for (const auto& unit: units)
    {
        if (unit.getType().getValue()
            == OARadarIqUnit_type::eOARadarIqUnit_type::RADAR)
        {
            emit radarInfoUpdated(unit.getModel());
            m_radarUnitId = unit.getId();
            LOG_DEBUG << "set radar unit info";
        }
        else if (
            unit.getType().getValue()
            == OARadarIqUnit_type::eOARadarIqUnit_type::CAMERA)
        {
            emit cameraInfoUpdated(unit.getModel());
            m_cameraUnitId = unit.getId();
            LOG_DEBUG << "set camera unit info";
        }
    }
}

void RadarIqConnection::parseServiceSettings(
    const OARadarIqService_settings& content)
{
    LOG_TRACE << "settings: " << content.asJsonObject();
    if (!content.isValid())
    {
        LOG_WARNING << "settings data is not valid";
        return;
    }

    emit generalSettingsUpdated(content.getGeneral());

    const auto& radarSettings = content.getRadar();
    emit radarPositionUpdated(radarSettings.getPosition());
    emit radarWorkzoneUpdated(radarSettings.getWorkzone().getSectors());
    emit radarPowerStatusReceived(
        radarSettings.isPowerOn() && radarSettings.isConnectionOn());

    const auto& cameraSettings = content.getCamera();
    emit cameraPositionUpdated(cameraSettings.getPosition());
    emit cameraConnectionStatusReceived(cameraSettings.isConnectionOn());
}

void RadarIqConnection::parseRadarTelemetry(
    const OARadarIqGet_radar_telemetry_200_response& content)
{
    LOG_TRACE << "telemetry: " << content.asJsonObject();
    if (!content.isValid())
    {
        LOG_WARNING << "telemetry data is not valid";
        return;
    }

    const auto telemetryData = content.getData();
    emit radarTemperatureUpdated(telemetryData.getTemperature());
    emit radarModeUpdated(telemetryData.getMode());
}

void RadarIqConnection::handleTracksMessage(const QJsonObject& json)
{
    LOG_TRACE << "tracks message";
    const auto& msgTracksData =
        json[QString(messageTable.at(MessageTag::Tracks).field)];
    QList<OARadarIqTrack> tracks;
    auto tracksValid = OARadarIq::fromJsonValue(tracks, msgTracksData);
    if (msgTracksData.isNull() || !tracksValid)
    {
        LOG_WARNING << "tracks data is not valid";
        return;
    }

    bool isImitator =
        tracks.front().getId().getSource() == 0
        && std::adjacent_find(
               tracks.begin(),
               tracks.end(),
               [](const auto& a, const auto& b)
               { return b.getId().getSource() != a.getId().getSource() + 1; })
               == tracks.end();
    if (isImitator != m_isImitator)
    {
        emit imitatorDetected(isImitator);
        m_isImitator = isImitator;
    }

    for (const auto constTracks = tracks; const auto& track: constTracks)
    {
        auto [valid, isRadar] = unitIdToDevice(track.getId().getUnit());
        if (!valid)
        {
            continue;
        }
        emit trackUpdated(isRadar, track);
    }
}

void RadarIqConnection::handleTracksRemoveMessage(const QJsonObject& json)
{
    LOG_TRACE << "tracks remove message";
    const auto& msgTracksRemoveData =
        json[QString(messageTable.at(MessageTag::RemoveTracks).field)];
    QList<OARadarIqTrack_id> tracks;
    auto tracksValid = OARadarIq::fromJsonValue(tracks, msgTracksRemoveData);
    if (msgTracksRemoveData.isNull() || !tracksValid)
    {
        LOG_WARNING << "tracks remove data is not valid";
        return;
    }

    for (const auto constTracks = tracks; const auto& track: constTracks)
    {
        auto [valid, _] = unitIdToDevice(track.getUnit());
        if (!valid)
        {
            continue;
        }
        emit trackRemoved(track);
    }
}

void RadarIqConnection::handlePositionMessage(const QJsonObject& json)
{
    const auto& msgPositionData =
        json[QString(messageTable.at(MessageTag::RadarPosition).field)];
    OARadarIqGeo_position position;
    auto positionValid = OARadarIq::fromJsonValue(position, msgPositionData);
    if (msgPositionData.isNull() || !positionValid)
    {
        LOG_WARNING << "position is not valid";
    }

    emit radarPositionUpdated(position);
    emit cameraPositionUpdated(position);
}

void RadarIqConnection::handleWorkzoneMessage(const QJsonObject& json)
{
    const auto& msgWorkzoneData =
        json[QString(messageTable.at(MessageTag::RadarWorkzone).field)];
    OARadarIqWorkzone workzone;
    auto workzoneValid = OARadarIq::fromJsonValue(workzone, msgWorkzoneData);
    if (msgWorkzoneData.isNull() || !workzoneValid)
    {
        LOG_WARNING << "workzone is not valid";
    }

    emit radarWorkzoneUpdated(workzone.getSectors());
}

void RadarIqConnection::handleTelemetryMessage(const QJsonObject& json)
{
    const auto& radarTelemetryMsg = messageTable.at(MessageTag::RadarTelemetry);
    auto msgTelemetryData = json[radarTelemetryMsg.field].toString();

    if (radarTelemetryMsg.tags)
    {
        for (const auto& [key, field]: *radarTelemetryMsg.tags)
        {
            if (msgTelemetryData == key)
            {
                if (key == temperatureTag)
                {
                    double temperature = json[field].toDouble();
                    emit radarTemperatureUpdated(temperature);
                }
                else if (key == modeTag)
                {
                    auto modeStr = json[field].toString();
                    OARadarIqRadar_mode mode(modeStr);
                    emit radarModeUpdated(mode);
                }
            }
        }
    }
}

} // namespace mpk::dss::backend::radariq
