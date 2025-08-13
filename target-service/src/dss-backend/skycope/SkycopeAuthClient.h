#pragma once

#include "OASkycopeDefaultApi.h"

#include "dss-common/config/AuthSettings.h"

#include <QObject>
#include <QString>

#include <chrono>
#include <memory>

namespace mpk::dss::backend::skycope
{
using namespace OASkycope;

class SkycopeAuthClient : public QObject
{
    Q_OBJECT

public:
    SkycopeAuthClient(
        std::shared_ptr<OASkycopeDefaultApi> apiClient,
        const config::AuthSettings& authSettings,
        QObject* parent = nullptr);

    void authenticate();
    bool ensureAuthenticated();

signals:
    void authenticated(bool initial);
    void authenticationFailed(std::chrono::milliseconds retryDelay);

private slots:
    void parseLogin(const OASkycopeAuth_token&);

private:
    QString m_token;
    std::chrono::milliseconds m_reAuthBackoff;
    std::shared_ptr<OASkycopeDefaultApi> m_apiClient;
};

} // namespace mpk::dss::backend::skycope
