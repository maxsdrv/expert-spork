#include "httpws/AuthTokens.h"

using namespace http;

QJsonValue ToJsonConverter<AuthTokens>::convert(const AuthTokens& value)
{
    return QJsonObject{
        {ACCESS_TOKEN, json::toValue(value.accessToken)},
        {REFRESH_TOKEN, json::toValue(value.refreshToken)}};
}

AuthTokens FromJsonConverter<AuthTokens>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    auto access = json::fromObject<Token>(object, ACCESS_TOKEN);
    auto refresh = json::fromObject<Token>(object, REFRESH_TOKEN);
    return AuthTokens{access, refresh};
}

QJsonValue ToJsonConverter<http::AuthTokensMap>::convert(const http::AuthTokensMap& map)
{
    QJsonObject result;
    for (const auto& pair: map)
    {
        result.insert(pair.first, json::toValue(pair.second));
    }
    return result;
}

http::AuthTokensMap FromJsonConverter<http::AuthTokensMap>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    http::AuthTokensMap tokens;
    for (auto it = object.constBegin(); it != object.constEnd(); ++it)
    {
        try
        {
            const auto name = it.key();
            const auto value = json::fromValue<std::vector<http::AuthTokens>>(it.value());
            tokens.insert({name, value});
        }
        // NOLINTNEXTLINE (bugprone-empty-catch)
        catch (const std::exception& ex)
        {
            // Nothing to do here
        }
    }
    return tokens;
}
