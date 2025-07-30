#include "mpk/rest/AuthTokenStorage.h"

#include "httpws/AuthService.h"
#include "httpws/AuthType.h"
#include "httpws/HttpException.h"

#include "json/JsonParser.h"
#include "json/JsonUtilities.h"
#include "log/Log2.h"
#include "qt/Files.h"
#include "qttypes/QStringConverters.h"

#include <QFileInfo>

#include <algorithm>

namespace
{

constexpr auto usersTag = "users";

template <class Tokens, class Member>
decltype(Tokens{}.begin()) tokensIt(Tokens& tokens, Member member, const QString& tokenValue)
{
    return std::find_if(
        tokens.begin(),
        tokens.end(),
        [tokenValue, member](const http::AuthTokens& tokens)
        { return (tokens.*member).value == tokenValue; });
}

void removeExpiredTokens(http::AuthTokensVector& tokens)
{
    auto begin = std::remove_if(
        tokens.begin(),
        tokens.end(),
        [](const http::AuthTokens& tokens) { return !testToken(tokens.refreshToken); });

    tokens.erase(begin, tokens.end());
}

auto generateTokens()
{
    using namespace http;
    return AuthTokens{
        Token::generate(http::ACCESS_TOKEN_LIFETIME),
        Token::generate(http::REFRESH_TOKEN_LIFETIME)};
}

void safeJsonToFile(const QJsonObject& object, const QString& filePath)
{
    try
    {
        json::toFile(object, filePath);
    }
    catch (std::exception& ex)
    {
        LOG_WARNING << "Unable to cache tokens: " << ex.what();
    }
}

} // namespace

namespace mpk::rest
{

AuthTokenStorage::AuthTokenStorage(
    gsl::not_null<http::AuthService*> authService,
    gsl::not_null<UserPermissionsProvider*> userPermissions,
    const std::filesystem::path& cacheFilePath) :
  m_authService(authService),
  m_userPermissions(userPermissions),
  m_cacheFilePath{qt::fromFilesystemPath(cacheFilePath)}
{
    if (QFileInfo::exists(m_cacheFilePath))
    {
        LOG_INFO << "Restore tokens from cache";
        try
        {
            auto json = json::fromFile(m_cacheFilePath);
            m_tokens = json::fromObject<decltype(m_tokens)>(json, usersTag);
        }
        catch (std::exception& ex)
        {
            LOG_WARNING << "Unable to load cached tokens: " << ex.what();
        }
    }
}

http::AuthTokens AuthTokenStorage::getTokens(const http::Credentials& credentials)
{
    LOG_DEBUG << "Trying to authenticate as " << credentials.login;
    if (m_authService->validate(credentials.login, credentials.password))
    {
        LOG_INFO << "Authenticated as " << credentials.login << " by password";
        auto tokens = generateTokens();

        auto& userTokens = m_tokens[credentials.login];
        removeExpiredTokens(userTokens);

        userTokens.push_back(tokens);

        safeJsonToFile(QJsonObject{{usersTag, json::toValue(m_tokens)}}, m_cacheFilePath);

        return tokens;
    }

    LOG_INFO << "Authentication as " << credentials.login << " by password failed";

    BOOST_THROW_EXCEPTION(
        exception::http::Exception()
        << exception::http::ResponseStatus(::http::HttpCode::NotAcceptable)
        << exception::http::ResponseMessage("Authentication failed"));
}

http::AuthTokens AuthTokenStorage::getTokens(const QString& refreshTokenValue)
{
    for (auto& [user, tokens]: m_tokens)
    {
        auto it = tokensIt(tokens, &http::AuthTokens::refreshToken, refreshTokenValue);
        if (it == tokens.end())
        {
            continue;
        }

        if (!testToken(it->refreshToken))
        {
            tokens.erase(it);
            break;
        }

        LOG_INFO << "Authenticated as " << user << " by token";
        it->accessToken = http::Token::generate(http::ACCESS_TOKEN_LIFETIME);

        safeJsonToFile(QJsonObject{{usersTag, json::toValue(m_tokens)}}, m_cacheFilePath);

        return *it;
    }

    LOG_INFO << "Authentication by token failed";

    BOOST_THROW_EXCEPTION(
        exception::http::Exception()
        << exception::http::ResponseStatus(::http::HttpCode::NotAcceptable));
}

http::Permissions AuthTokenStorage::validate(const QString& accessTokenValue) const
{
    auto it = std::find_if(
        m_tokens.begin(),
        m_tokens.end(),
        [accessTokenValue](const auto& pair)
        {
            const auto& tokens = pair.second;
            return tokensIt(tokens, &http::AuthTokens::accessToken, accessTokenValue)
                   != tokens.end();
        });

    if (it != m_tokens.end())
    {
        const auto& user = it->first;
        return m_userPermissions->list(user);
    }

    return http::Permissions{};
}

} // namespace mpk::rest
