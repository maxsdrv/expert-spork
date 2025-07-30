#pragma once

#include "Json.h"

#include <chrono>

namespace http
{

constexpr auto ACCESS_TOKEN = "access_token";
constexpr auto REFRESH_TOKEN = "refresh_token";

constexpr auto ACCESS_TOKEN_LIFETIME = std::chrono::minutes(30);
constexpr auto REFRESH_TOKEN_LIFETIME = std::chrono::hours(24 * 30);

struct Token
{
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<int64_t>;

    static Token generate(Duration lifetime);

    QString value;
    TimePoint expirationPoint;
};

bool testToken(const Token& token);
void resetToken(Token& token);
bool operator==(const Token& lhs, const Token& rhs);
bool operator!=(const Token& lhs, const Token& rhs);

} // namespace http

template <>
struct FromJsonConverter<http::Token>
{
    static http::Token get(const QJsonValue& json);
};

template <>
struct ToJsonConverter<http::Token>
{
    static QJsonValue convert(const http::Token& token);
};
