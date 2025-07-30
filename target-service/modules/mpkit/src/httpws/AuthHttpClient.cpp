#include "httpws/AuthHttpClient.h"

#include "httpws/AuthUtils.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"
#include "httpws/HttpRoute.h"

#include "log/Log2.h"

#include "httplib.h"

#include <QtConcurrentRun>

using namespace exception::http;

namespace http
{

namespace
{

constexpr auto DefaultConnectionTimeout = std::chrono::seconds(3);

// TODO: remove the workaround after all customers' Radar IQ SW upgraded
http::AuthTokens workaroundMigrationFromSteadyToSystemClockTokenExpirationTime(
    const http::AuthTokens& tokens)
{
    auto nowTime = Token::Clock::now();
    auto tokenExpTime = tokens.accessToken.expirationPoint.time_since_epoch().count();
    // if received and current timestamps differ by an order of magnitude - it's definitely broken
    // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
    if (tokenExpTime * 10 < nowTime.time_since_epoch().count())
    {
        LOG_WARNING << "Reset tokens' expiration time relative to the current time";
        auto newTokens = http::AuthTokens{};
        newTokens.accessToken.value = tokens.accessToken.value;
        newTokens.refreshToken.value = tokens.refreshToken.value;
        newTokens.accessToken.expirationPoint = nowTime + ACCESS_TOKEN_LIFETIME;
        newTokens.refreshToken.expirationPoint = nowTime + REFRESH_TOKEN_LIFETIME;
        return newTokens;
    }
    return tokens;
}

} // namespace

AuthHttpClient::AuthHttpClient(
    ConnectionSettings connectionSettings, Credentials credentials, QObject* parent) :
  QObject(parent),
  m_connectionSettings(std::move(connectionSettings)),
  m_credentials(std::move(credentials))
{
}

QFuture<http::AuthTokens> AuthHttpClient::authenticate(const Token& refreshToken)
{
    return QtConcurrent::run(
        [this, refreshToken]()
        {
            try
            {
                auto connection = createConnection(refreshToken);

                auto authPath = resolvePath(API_VERSION, loginPath);
                auto authResult = connection.Post(authPath.toLatin1().data());

                checkResult(authResult);

                auto tokens = parseAuthResponseBody(QByteArray::fromStdString(authResult->body));
                tokens = workaroundMigrationFromSteadyToSystemClockTokenExpirationTime(tokens);

                emit authenticated(tokens);
                return tokens;
            }
            catch (const Exception& ex)
            {
                LOG_WARNING << "Authentication request failed. Error = " << httpErrorInfo(ex)
                            << ", status = " << static_cast<int>(responseErrorStatus(ex));
            }
            catch (const std::exception& ex)
            {
                LOG_WARNING << "Authentication request failed with a code = "
                            << static_cast<int>(HttpCode::InternalServerError) << " "
                            << ex.what();
            }

            emit authenticationFailed();
            return http::AuthTokens{};
        });
}

httplib::Client AuthHttpClient::createConnection(const Token& refreshToken) const
{
    httplib::Client client(m_connectionSettings.host.toStdString(), m_connectionSettings.port);
    client.set_connection_timeout(DefaultConnectionTimeout.count(), 0);

    if (!refreshToken.value.isEmpty())
    {
        client.set_bearer_token_auth(refreshToken.value.toLatin1().data());
    }
    else
    {
        client.set_basic_auth(
            m_credentials.login.toStdString(), m_credentials.password.toStdString());
    }

    return client;
}

void AuthHttpClient::checkResult(const httplib::Result& result)
{
    if (result.error() != httplib::Error::Success)
    {
        int error = static_cast<int>(result.error());
        BOOST_THROW_EXCEPTION(
            Exception() << HttpErrorInfo(error)
                        << ResponseMessage(std::string("Authentication request failed")));
    }

    if (result->status != static_cast<int>(HttpCode::Ok))
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(static_cast<::http::HttpCode>(result->status))
                        << ResponseMessage(result->body));
    }

    if (result->get_header_value(contentTypeHeader) != jsonContentType)
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(HttpCode::UnsupportedMediaType)
                        << ResponseMessage(std::string("Response content type error")));
    }
}

void AuthHttpClient::setConnectionSettings(const ConnectionSettings& connectionSettings)
{
    m_connectionSettings = connectionSettings;
}

ConnectionSettings AuthHttpClient::connectionSettings() const
{
    return m_connectionSettings;
}

void AuthHttpClient::setCredentials(const Credentials& credentials)
{
    m_credentials = credentials;
}

Credentials AuthHttpClient::credentials() const
{
    return m_credentials;
}

} // namespace http
