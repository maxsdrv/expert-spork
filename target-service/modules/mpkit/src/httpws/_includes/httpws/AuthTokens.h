#pragma once

#include "Token.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

namespace http
{

struct AuthTokens
{
    Token accessToken;
    Token refreshToken;
};

using AuthTokensVector = std::vector<http::AuthTokens>;
using AuthTokensMap = std::map<QString, AuthTokensVector>;

} // namespace http

Q_DECLARE_METATYPE(http::AuthTokens) // NOLINT

template <>
struct ToJsonConverter<http::AuthTokens>
{
    static QJsonValue convert(const http::AuthTokens& value);
};

template <>
struct FromJsonConverter<http::AuthTokens>
{
    static http::AuthTokens get(const QJsonValue& value);
};

template <>
struct ToJsonConverter<http::AuthTokensMap>
{
    static QJsonValue convert(const http::AuthTokensMap& map);
};

template <>
struct FromJsonConverter<http::AuthTokensMap>
{
    static http::AuthTokensMap get(const QJsonValue& value);
};
