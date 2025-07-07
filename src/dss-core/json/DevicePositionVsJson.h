#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include "DevicePosition.h"

template <>
struct ToJsonConverter<mpk::dss::core::DevicePosition>
{
    static QJsonValue convert(const mpk::dss::core::DevicePosition& position);
};

template <>
struct FromJsonConverter<mpk::dss::core::DevicePosition>
{
    static mpk::dss::core::DevicePosition get(const QJsonValue& value);
};
