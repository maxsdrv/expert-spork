#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include "DeviceHwInfo.h"

template <>
struct ToJsonConverter<mpk::dss::core::DeviceHwInfo>
{
    static QJsonValue convert(const mpk::dss::core::DeviceHwInfo& hwInfo);
};

template <>
struct FromJsonConverter<mpk::dss::core::DeviceHwInfo>
{
    static mpk::dss::core::DeviceHwInfo get(const QJsonValue& value);
};
