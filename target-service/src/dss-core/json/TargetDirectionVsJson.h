#pragma once

#include "dss-core/TargetDirection.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::core::Direction>
{
    static QJsonValue convert(const mpk::dss::core::Direction& direction);
};

template <>
struct FromJsonConverter<mpk::dss::core::Direction>
{
    static mpk::dss::core::Direction get(const QJsonValue& value);
};
