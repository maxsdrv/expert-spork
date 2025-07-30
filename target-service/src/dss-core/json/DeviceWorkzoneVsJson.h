#pragma once

#include "dss-core/DeviceWorkzone.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::core::DeviceWorkzone>
{
    static QJsonValue convert(const mpk::dss::core::DeviceWorkzone& data);
};

template <>
struct FromJsonConverter<mpk::dss::core::DeviceWorkzone>
{
    static mpk::dss::core::DeviceWorkzone get(const QJsonValue& value);
};
