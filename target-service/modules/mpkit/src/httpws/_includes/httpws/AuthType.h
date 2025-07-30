#pragma once

#include <QString>

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

namespace http
{

struct AuthType
{
    enum Value : uint8_t
    {
        BASIC, /**< Auth with login & passwd */
        BEARER /**< Auth with token */
    };

    static QString toString(Value eValue);
    static Value fromString(const QString& eName);
};

struct Credentials
{
    QString login;
    QString password;
};

} // namespace http

template <>
struct ToJsonConverter<http::Credentials>
{
    static QJsonValue convert(const http::Credentials& value);
};

template <>
struct FromJsonConverter<http::Credentials>
{
    static http::Credentials get(const QJsonValue& value);
};
