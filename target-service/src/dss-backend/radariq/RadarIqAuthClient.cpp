#include "RadarIqAuthClient.h"

#include "httpws/AuthType.h"
#include "log/Log2.h"

namespace mpk::dss::backend::radariq
{

using namespace std::chrono_literals;
constexpr auto minReAuthBackoff = 100ms;
constexpr auto maxReAuthBackoff = std::chrono::milliseconds(10s);

namespace
{

} // namespace

RadarIqAuthClient::RadarIqAuthClient(
    const config::ConnectionSettings& httpSettings,
    const config::AuthSettings& authSettings,
    QObject* parent) :
  QObject(parent),
  m_reAuthBackoff{minReAuthBackoff},
  m_radarIqAuthClient{std::make_unique<http::AuthHttpClient>(
      httpSettings,
      http::Credentials{authSettings.username, authSettings.password})}
{
    qRegisterMetaType<http::AuthTokens>("http.AuthTokens");
    connect(
        m_radarIqAuthClient.get(),
        &http::AuthHttpClient::authenticated,
        this,
        &RadarIqAuthClient::handleAuthSuccess);

    connect(
        m_radarIqAuthClient.get(),
        &http::AuthHttpClient::authenticationFailed,
        this,
        &RadarIqAuthClient::handleAuthFailure);
}

void RadarIqAuthClient::authenticate() const
{
    m_radarIqAuthClient->authenticate();
}

void RadarIqAuthClient::handleAuthSuccess(const http::AuthTokens& tokens)
{
    LOG_DEBUG << "authenticated";

    emit authenticated(
        tokens.accessToken.value, m_tokens.accessToken.value.isEmpty());
    m_tokens = tokens;

    m_reAuthBackoff = minReAuthBackoff;
}

void RadarIqAuthClient::handleAuthFailure()
{
    LOG_ERROR << "auth failed. Retry after " << m_reAuthBackoff.count()
              << " ms";
    emit authenticationFailed(m_reAuthBackoff);

    if (!m_tokens.refreshToken.value.isEmpty())
    {
        LOG_WARNING << "refreshing auth token failed";
        m_tokens.refreshToken.value.clear();
    }
    m_reAuthBackoff = std::min(m_reAuthBackoff * 2, maxReAuthBackoff);
}

bool RadarIqAuthClient::ensureAuthenticated()
{
    if (m_tokens.accessToken.value.isEmpty())
    {
        LOG_DEBUG << "auth retry";
        m_radarIqAuthClient->authenticate(m_tokens.refreshToken);
        return false;
    }
    const auto now = http::Token::Clock::now();
    const auto exp = m_tokens.accessToken.expirationPoint;

    LOG_TRACE << "token exp: "
              << std::chrono::duration_cast<std::chrono::seconds>(
                     exp.time_since_epoch())
                     .count()
              << " now: "
              << std::chrono::duration_cast<std::chrono::seconds>(
                     now.time_since_epoch())
                     .count();

    if (now > exp - std::chrono::minutes(1))
    {
        auto lifetimeSeconds =
            std::chrono::duration_cast<std::chrono::seconds>(exp - now).count();
        LOG_DEBUG << "refreshing access token before " << lifetimeSeconds
                  << " seconds to expiration";
        m_radarIqAuthClient->authenticate(m_tokens.refreshToken);
        if (lifetimeSeconds < 0)
        {
            LOG_WARNING << "expired access token";
            m_tokens.accessToken.value.clear();
        }
        return false;
    }
    return true;
}

} // namespace mpk::dss::backend::radariq
