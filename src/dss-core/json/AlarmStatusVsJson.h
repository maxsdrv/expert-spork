#pragma once

#include "dss-core/AlarmStatus.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::core::AlarmStatus>
{
    static QJsonValue convert(const mpk::dss::core::AlarmStatus& alarmStatus);
};

template <>
struct FromJsonConverter<mpk::dss::core::AlarmStatus>
{
    static mpk::dss::core::AlarmStatus get(const QJsonValue& value);
};
