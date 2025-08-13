#pragma once

#include "RadarIqAuthClient.h"

#include "OARadarIqCameraApi.h"
#include "OARadarIqDeviceApi.h"
#include "OARadarIqRadarApi.h"
#include "OARadarIqTrack.h"
#include "OARadarIqTrack_id.h"

#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/ConnectionSettings.h"

#include "mpk/rest/HttpWsConnection.h"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <memory>

namespace mpk::dss::backend::radariq
{
// using namespace OAService;
using namespace OARadarIq;

class RadarIqConnection : public QObject
{
    Q_OBJECT

public:
    RadarIqConnection(
        const config::ConnectionSettings& httpConnectionSettings,
        const config::ConnectionSettings& wsConnectionSettings,
        const std::chrono::milliseconds& updateIntervalSettings,
        const std::chrono::milliseconds& timeoutSettings,
        const config::AuthSettings& authSettings,
        QObject* parent);

    void setDisabled(bool disabled);

signals:
    void updateDisabled();
    void updateStarted(bool connected, bool faultless);

    void generalSettingsUpdated(OARadarIqDevice_general_settings settings);

    void radarInfoUpdated(QString model);
    void radarPositionUpdated(OARadarIqGeo_position position);
    void radarWorkzoneUpdated(QList<OARadarIqSector> workzone);
    void radarTemperatureUpdated(double temperature);
    void radarModeUpdated(OARadarIqRadar_mode mode);
    void radarPowerStatusReceived(bool isPowerOn);

    void cameraInfoUpdated(QString model);
    void cameraPositionUpdated(OARadarIqGeo_position position);
    void cameraConnectionStatusReceived(bool isConnection);

    void trackUpdated(bool isRadar, OARadarIqTrack track);
    void trackRemoved(OARadarIqTrack_id trackId);
    void imitatorDetected(bool isImitator);

private slots:
    void toggleUpdateDisabled();
    void handleAuthSuccess(const QString& accessToken, bool initial);
    void handleAuthFailure(std::chrono::milliseconds);
    void updateState();
    void parseUnitInfoList(const OARadarIqUnit_info_list&);
    void parseServiceSettings(const OARadarIqService_settings&);
    void parseRadarTelemetry(const OARadarIqGet_radar_telemetry_200_response&);
    void handleTracksMessage(const QJsonObject& json);
    void handleTracksRemoveMessage(const QJsonObject& json);
    void handlePositionMessage(const QJsonObject& json);
    void handleWorkzoneMessage(const QJsonObject& json);
    void handleTelemetryMessage(const QJsonObject& json);

private:
    void registerHandlers();

    [[nodiscard]] bool isConnectionOk() const;
    [[nodiscard]] bool isFaultless() const;
    void updateDeviceState();
    void getRadarIqData(bool initial = false);
    [[nodiscard]] std::pair<bool, bool> unitIdToDevice(QString unitId) const;

private:
    QString m_radarUnitId;
    QString m_cameraUnitId;
    bool m_disabled = false;
    bool m_isImitator = false;
    std::chrono::milliseconds m_updateInterval;
    std::unique_ptr<rest::HttpWsConnection> m_wsConnection;
    QTimer m_httpTimer;

    std::unique_ptr<OARadarIqDeviceApi> m_deviceApiClient;
    std::unique_ptr<OARadarIqRadarApi> m_radarApiClient;
    std::unique_ptr<OARadarIqCameraApi> m_cameraApiClient;
    bool m_httpTimeout{false};
    bool m_httpError{false};

    std::unique_ptr<RadarIqAuthClient> m_authClient;
};

} // namespace mpk::dss::backend::radariq
