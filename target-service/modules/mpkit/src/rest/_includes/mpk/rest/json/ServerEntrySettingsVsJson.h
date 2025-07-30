#pragma once

#include "mpk/rest/ServerEntrySettings.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::rest::ServerEntrySettings>
{
    static QJsonValue convert(const mpk::rest::ServerEntrySettings& value);
};

template <>
struct FromJsonConverter<mpk::rest::ServerEntrySettings>
{
    static mpk::rest::ServerEntrySettings get(const QJsonValue& value);
};
