#include "SkycopeAuthClient.h"

#include "log/Log2.h"

namespace mpk::dss::backend::skycope
{

using namespace std::chrono_literals;
constexpr auto minReAuthBackoff = 100ms;
constexpr auto maxReAuthBackoff = std::chrono::milliseconds(10s);

namespace
{

using Clock = std::chrono::system_clock;
auto extractExpired(const QString& jwtToken) -> std::chrono::time_point<Clock>
{
    auto jwtElements = jwtToken.split(".");
    const auto& b64Header = jwtElements.at(0);
    QByteArray byteHeaderBase64{b64Header.toUtf8()};
    auto fromBase64Result = QByteArray::fromBase64Encoding(byteHeaderBase64);
    if (fromBase64Result.decodingStatus != QByteArray::Base64DecodingStatus::Ok)
    {
        LOG_ERROR << "error decode base64 jwt token header";
        return Clock::time_point{};
    }
    QString decodedHeader{fromBase64Result.decoded};
    QJsonParseError error{};
    auto jsonDocHeader =
        QJsonDocument::fromJson(decodedHeader.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        LOG_ERROR << "error parse json jwt header: " << error.errorString();
        return Clock::time_point{};
    }
    auto expVal = jsonDocHeader["exp"].toInt();
    Clock::time_point exp{std::chrono::seconds(expVal)};

    return exp;
}

} // namespace

SkycopeAuthClient::SkycopeAuthClient(
    std::shared_ptr<OASkycopeDefaultApi> apiClient,
    const config::AuthSettings& authSettings,
    QObject* parent) :
  QObject(parent),
  m_reAuthBackoff{minReAuthBackoff},
  m_apiClient{std::move(apiClient)}
{
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::loginSignal,
        this,
        &SkycopeAuthClient::parseLogin);
    connect(
        m_apiClient.get(),
        &OASkycopeDefaultApi::loginSignalError,
        this,
        [this](const auto&, const auto&, const auto& errorStr)
        {
            LOG_ERROR << "Auth failed: " << errorStr << " Retry after "
                      << m_reAuthBackoff.count() << " ms";

            emit authenticationFailed(m_reAuthBackoff);
            if (!m_token.isEmpty())
            {
                LOG_WARNING << "Refreshing auth token failed";
                m_token.clear();
            }
            m_reAuthBackoff = std::min(m_reAuthBackoff * 2, maxReAuthBackoff);
        });

    m_apiClient->setUsername(authSettings.username);
    m_apiClient->setPassword(authSettings.password);
}

void SkycopeAuthClient::authenticate()
{
    m_apiClient->login();
}

void SkycopeAuthClient::parseLogin(const OASkycopeAuth_token& content)
{
    LOG_TRACE << "login: " << content.asJsonObject();
    if (!content.isValid())
    {
        LOG_ERROR << "login data is not valid";
        m_reAuthBackoff = std::min(m_reAuthBackoff * 2, maxReAuthBackoff);
        emit authenticationFailed(m_reAuthBackoff);
        return;
    }

    emit authenticated(m_token.isEmpty());
    m_token = content.getToken();

    QByteArray b64;
    b64.append(m_token.toUtf8() + ":");
    m_apiClient->addHeaders("Authorization", "Basic " + b64.toBase64());

    m_reAuthBackoff = minReAuthBackoff;
}

bool SkycopeAuthClient::ensureAuthenticated()
{
    if (m_token.isEmpty())
    {
        LOG_DEBUG << "Auth retry";
        m_apiClient->login();
        return false;
    }
    const auto now = Clock::now();
    const auto exp = extractExpired(m_token);

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
        m_apiClient->login();
        if (lifetimeSeconds < 0)
        {
            LOG_WARNING << "Expired access token";
            m_token.clear();
            return false;
        }
    }
    return true;
}

} // namespace mpk::dss::backend::skycope
