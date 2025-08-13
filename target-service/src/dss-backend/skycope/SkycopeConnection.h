#pragma once

#include "SkycopeAuthClient.h"

#include "OASkycopeDefaultApi.h"

#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/ConnectionSettings.h"
#include "dss-core/JammerMode.h"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <memory>

namespace mpk::dss::backend::skycope
{

class SkycopeConnection : public QObject
{
    Q_OBJECT

public:
    SkycopeConnection(
        const config::ConnectionSettings& httpConnectionSettings,
        const std::chrono::milliseconds& updateIntervalSettings,
        const std::chrono::milliseconds& timeoutSettings,
        const config::AuthSettings& authSettings,
        QObject* parent = nullptr);

    void setUpdateDisabled(bool disabled);
    void postAutoDefense(const QString& id) const;
    void deleteAutoDefense(const QString& id) const;
    void postDefenseDataStart(
        const QString& sensorId,
        qint32 azimuth,
        qint32 sDuration,
        const QList<QString>& frequency,
        const QString& strategy);
    void postDefenseDataStop(const QString& sensorId);

signals:
    void httpTimerDisabled(bool disabled);
    void updateStarted(bool connected, bool faultless);
    void systemDataUpdated(OASkycopeModel content);
    void defenseDataUpdated(
        QMap<QString, OASkycopeMassDefense> content, bool initial);
    void postDefenseDataSignalError();

private slots:
    void toggleUpdateDisabled(bool disabled);
    void handleAuthSuccess(bool);
    void handleAuthFailure(std::chrono::milliseconds);
    void updateState();
    void parseSystemData(const OASkycopeModel&);
    void parseDefenseData(QMap<QString, OASkycopeMassDefense>);

private:
    void registerHandlers();

    [[nodiscard]] bool isConnectionOk() const;
    [[nodiscard]] bool isFaultless() const;
    void updateDeviceState();
    void getSkycopeData(bool initial = false);

private:
    std::chrono::milliseconds m_updateInterval;
    QTimer m_httpTimer;
    bool m_initial = false;

    QString m_sessionUuid;
    std::shared_ptr<OASkycopeDefaultApi> m_apiClient;
    std::unique_ptr<SkycopeAuthClient> m_authClient;
    bool m_httpTimeout{false};
    bool m_httpError{false};
};

} // namespace mpk::dss::backend::skycope
