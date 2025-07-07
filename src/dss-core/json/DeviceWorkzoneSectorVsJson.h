#pragma once

#include "dss-core/DeviceWorkzoneSector.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::core::DeviceWorkzoneSector>
{
    static QJsonValue convert(const mpk::dss::core::DeviceWorkzoneSector& data);
};

template <>
struct FromJsonConverter<mpk::dss::core::DeviceWorkzoneSector>
{
    static mpk::dss::core::DeviceWorkzoneSector get(const QJsonValue& value);
};
