#pragma once

#include "dss-core/Track.h"

#include "json/JsonComposer.h"

template <>
struct ToJsonConverter<mpk::dss::core::Track>
{
    static QJsonValue convert(const mpk::dss::core::Track& track);
};

template <>
struct ToJsonConverter<mpk::dss::core::TrackPoint>
{
    static QJsonValue convert(const mpk::dss::core::TrackPoint& trackPoint);
};
