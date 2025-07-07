#pragma once

#include "dss-backend/rest/SensorTargetData.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::SensorTargetData>
{
    static QJsonValue convert(
        const mpk::dss::backend::rest::SensorTargetData& targetData);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::SensorTargetData>
{
    static mpk::dss::backend::rest::SensorTargetData get(
        const QJsonValue& value);
};
