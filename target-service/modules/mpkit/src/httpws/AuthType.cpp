#include "httpws/AuthType.h"

#include "utils/EnumNamesMapper.h"

namespace
{

constexpr auto loginTag = "login";

auto authTypes()
{
    static const auto data = std::unordered_map<QString, http::AuthType::Value>{
        {"Basic", http::AuthType::Value::BASIC}, {"Bearer", http::AuthType::Value::BEARER}};
    return data;
}

} // namespace

namespace http
{

QString AuthType::toString(AuthType::Value eValue)
{
    return enums::toString(authTypes(), eValue);
}

AuthType::Value AuthType::fromString(const QString& eName)
{
    return enums::fromString(authTypes(), eName);
}

} // namespace http

QJsonValue ToJsonConverter<http::Credentials>::convert(const http::Credentials& value)
{
    /** Password never stored */
    return QJsonObject{{loginTag, json::toValue(value.login)}};
}

http::Credentials FromJsonConverter<http::Credentials>::get(const QJsonValue& value)
{
    /** Password never stored */
    auto object = json::fromValue<QJsonObject>(value);
    auto login = json::fromObject<QString>(object, loginTag);
    return http::Credentials{login, QString()};
}
