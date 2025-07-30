#pragma once

#include "SensorDevice.h"

#include <json/JsonParser.h>
#include "json/JsonComposer.h"

template <>
struct ToJsonConverter<mpk::dss::core::SensorDevice>
{
    static QJsonValue convert(const mpk::dss::core::SensorDevice& sensor);
};
