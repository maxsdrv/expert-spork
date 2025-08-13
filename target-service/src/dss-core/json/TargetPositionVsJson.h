#pragma once

#include "dss-core/TargetPosition.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::core::TargetPosition>
{
    static QJsonValue convert(const mpk::dss::core::TargetPosition& position);
};

template <>
struct FromJsonConverter<mpk::dss::core::TargetPosition>
{
    static mpk::dss::core::TargetPosition get(const QJsonValue& value);
};
