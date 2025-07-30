#pragma once

#include "mpk/rest/EntrySettings.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::rest::EntrySettings>
{
    static QJsonValue convert(const mpk::rest::EntrySettings& value);
};

template <>
struct FromJsonConverter<mpk::rest::EntrySettings>
{
    static mpk::rest::EntrySettings get(const QJsonValue& value);
};
