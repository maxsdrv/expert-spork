#pragma once

#include "dss-core/TargetData.h"

#include "json/JsonComposer.h"

template <>
struct ToJsonConverter<mpk::dss::core::TargetData>
{
    static QJsonValue convert(const mpk::dss::core::TargetData& data);
};
