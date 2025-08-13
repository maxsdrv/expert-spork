#pragma once

#include "dss-core/JammerInfo.h"
#include "dss-core/JammerTimeoutStatus.h"

#include <json/JsonComposer.h>
#include <json/JsonParser.h>

template <>
struct ToJsonConverter<mpk::dss::core::JammerBand>
{
    static QJsonValue convert(const mpk::dss::core::JammerBand& band);
};

template <>
struct FromJsonConverter<mpk::dss::core::JammerBand>
{
    static mpk::dss::core::JammerBand get(const QJsonValue& value);
};

template <>
struct ToJsonConverter<mpk::dss::core::JammerInfo>
{
    static QJsonValue convert(const mpk::dss::core::JammerInfo& info);
};

template <>
struct ToJsonConverter<mpk::dss::core::JammerTimeoutStatus>
{
    static QJsonValue convert(const mpk::dss::core::JammerTimeoutStatus& status);
};
