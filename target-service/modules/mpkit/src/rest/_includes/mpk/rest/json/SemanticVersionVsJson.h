#pragma once

#include "mpk/rest/SemanticVersion.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::rest::SemanticVersion>
{
    static QJsonValue convert(const mpk::rest::SemanticVersion& value);
};

template <>
struct FromJsonConverter<mpk::rest::SemanticVersion>
{
    static mpk::rest::SemanticVersion get(const QJsonValue& value);
};
