#pragma once

#include "httpws/ConnectionSettings.h"

#include "json/FromJson.h"
#include "json/ToJson.h"

#include <QString>

template <>
struct ToJsonConverter<http::ConnectionSettings>
{
    static QJsonValue convert(const http::ConnectionSettings& settings);
};

template <>
struct FromJsonConverter<http::ConnectionSettings>
{
    static http::ConnectionSettings get(const QJsonValue& value);
};
