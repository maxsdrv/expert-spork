#include "SkycopeConnection.h"

#include "dss-common/api/Helpers.h"
#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/HttpTags.h"
#include "dss-common/config/Option.h"
#include "dss-core/SensorDevice.h"
#include "dss-core/YamlConvertHelper.h"

#include <QNetworkReply>
#include <QUuid>

namespace mpk::dss::backend::skycope
{

using namespace std::chrono_literals;

SkycopeConnection::SkycopeConnection(
    const config::ConnectionSettings& httpConnectionSettings,
    const std::chrono::milliseconds& updateIntervalSettings,
    const std::chrono::milliseconds& timeoutSettings,
    const config::AuthSettings& authSettings,
    QObject* parent) :
  QObject(parent),
  m_updateInterval{updateIntervalSettings},
  m_sessionUuid(QUuid::createUuid().toString()),
  m_apiClient{std::make_shared<OASkycopeDefaultApi>(timeoutSettings.count())},
  m_authClient{
      std::make_unique<SkycopeAuthClient>(m_apiClient, authSettings, this)}
{
    registerHandlers();

    const QString deviceUrl = "http://" + httpConnectionSettings.host + ":"
                              + QString::number(httpConnectionSettings.port)
                              + "/api/v4.1";
    LOG_INFO << "SkycopeConnection(): remote device URL: " << deviceUrl;
    m_apiClient->setNewServerForAllOperations(deviceUrl);

    m_authClient->authenticate();
}

void SkycopeConnection::registerHandlers()
{
    connect(
        &m_httpTimer, &QTimer::timeout, this, &SkycopeConnection::updateState);

    connect(
        this,
        &SkycopeConnection::httpTimerDisabled,
        this,
        &SkycopeConnection::toggleUpdateDisabled);

    connect(
        m_authClient.get(),
        &SkycopeAuthClient::authenticated,
        this,
        &SkycopeConnection::handleAuthSuccess);
    connect(
        m_authClient.get(),
        &SkycopeAuthClient::authenticationFailed,
        this,
        &SkycopeConnection::handleAuthFailure);

    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::getDataSignal,
        this,
        &SkycopeConnection::parseSystemData);
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::getDataSignalError,
        this,
        [this](const auto&, const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "getData", m_httpTimeout, m_httpError, errorType, errorStr);
        });
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::getDefenseDataSignal,
        this,
        &SkycopeConnection::parseDefenseData);
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::getDefenseDataSignalError,
        this,
        [this](const auto&, const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "getDefenseData",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::postDefenseDataSignalError,
        this,
        [this](const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "postDefenseData",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::postAutoDefenseSignalError,
        this,
        [this](const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "postAutoDefense",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::deleteAutoDefenseSignalError,
        this,
        [this](const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "deleteAutoDefense",
                m_httpTimeout,
                m_httpError,
                errorType,
                errorStr);
        });

    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::postDataSignalError,
        this,
        [this](const auto& errorType, const auto& errorStr)
        {
            api::handleRequestFailure(
                "postData", m_httpTimeout, m_httpError, errorType, errorStr);
        });

    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::postDefenseDataSignalError,
        this,
        &SkycopeConnection::postDefenseDataSignalError);
}

void SkycopeConnection::setUpdateDisabled(bool disabled)
{
    emit httpTimerDisabled(disabled);
}

void SkycopeConnection::toggleUpdateDisabled(bool disabled)
{
    disabled ? m_httpTimer.stop() : m_httpTimer.start();
}

void SkycopeConnection::handleAuthSuccess(bool initial)
{
    LOG_DEBUG << "Authenticated";

    if (initial)
    {
        m_httpTimer.start(m_updateInterval);
        getSkycopeData(true);
        emit updateStarted(
            /*connected=*/true,
            /*faultless=*/true);
    }
    m_httpTimer.setSingleShot(false);
}

void SkycopeConnection::handleAuthFailure(std::chrono::milliseconds retryDelay)
{
    m_httpTimer.setSingleShot(true);
    m_httpTimer.start(retryDelay);

    emit updateStarted(
        /*connected=*/false,
        /*faultless=*/true);
}

void SkycopeConnection::updateState()
{
    emit updateStarted(isConnectionOk(), isFaultless());
    if (!m_authClient->ensureAuthenticated())
    {
        return;
    }
    getSkycopeData();
}

void SkycopeConnection::getSkycopeData(bool initial)
{
    m_initial = initial;
    m_httpTimeout = false;
    m_httpError = false;

    m_apiClient->getData(
        "1", // full=
        "0", // ts=
        m_sessionUuid);

    m_apiClient->getDefenseData();
}

void SkycopeConnection::parseSystemData(const OASkycopeModel& content)
{
    if (!content.isValid())
    {
        LOG_WARNING << "parseSystemData: System data is not valid";
        return;
    }
    emit systemDataUpdated(content);
}

void SkycopeConnection::parseDefenseData(
    QMap<QString, OASkycopeMassDefense> content)
{
    for (auto it = content.keyValueBegin(); it != content.keyValueEnd(); ++it)
    {
        LOG_TRACE << "parseDefenseData[" << it->first
                  << "]: " << it->second.asJson();
        if (!it->second.isValid())
        {
            LOG_WARNING << "parseDefenseData: data is not valid";
            return;
        }
    }
    emit defenseDataUpdated(content, m_initial);
}

bool SkycopeConnection::isConnectionOk() const
{
    return !m_httpTimeout;
}

bool SkycopeConnection::isFaultless() const
{
    return !m_httpError;
}

void SkycopeConnection::postAutoDefense(const QString& id) const
{
    m_apiClient->postAutoDefense(id);
}

void SkycopeConnection::deleteAutoDefense(const QString& id) const
{
    m_apiClient->deleteAutoDefense(id);
}

// TODO: test negative azimuth possible
void SkycopeConnection::postDefenseDataStart(
    const QString& sensorId,
    qint32 azimuth,
    qint32 sDuration,
    const QList<QString>& frequency,
    const QString& strategy)
{
    auto md = OASkycope::OASkycopeMassDefense();
    md.setAzimuth(azimuth);
    md.setDuration(sDuration);
    md.setFrequency(frequency);
    md.setStrategy(strategy);
    m_apiClient->postDefenseData(sensorId, md);
}

void SkycopeConnection::postDefenseDataStop(const QString& sensorId)
{
    Q_ASSERT(sensorId.count());
    m_apiClient->deleteMassDefense(sensorId);
}

} // namespace mpk::dss::backend::skycope
