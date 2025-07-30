#pragma once

#include "dss-common/config/AuthSettings.h"
#include "dss-common/config/ConnectionSettings.h"

#include "httpws/AuthHttpClient.h"
#include "httpws/AuthTokens.h"

#include <QObject>
#include <QString>

#include <chrono>
#include <memory>

namespace mpk::dss::backend::radariq
{

class RadarIqAuthClient : public QObject
{
    Q_OBJECT

public:
    RadarIqAuthClient(
        const config::ConnectionSettings& httpSettings,
        const config::AuthSettings& authSettings,
        QObject* parent = nullptr);

    void authenticate() const;
    bool ensureAuthenticated();

signals:
    void authenticated(QString accessToken, bool initial);
    void authenticationFailed(std::chrono::milliseconds retryDelay);

private slots:
    void handleAuthSuccess(const http::AuthTokens& tokens);
    void handleAuthFailure();

private:
    http::AuthTokens m_tokens;
    std::chrono::milliseconds m_reAuthBackoff;
    std::unique_ptr<http::AuthHttpClient> m_radarIqAuthClient;
};

} // namespace mpk::dss::backend::radariq
