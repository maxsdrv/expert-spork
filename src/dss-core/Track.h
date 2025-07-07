#pragma once

#include "dss-core/DeviceId.h"
#include "dss-core/TargetDirection.h"
#include "dss-core/TargetId.h"
#include "dss-core/TrackId.h"

#include <QGeoCoordinate>

#include <optional>

namespace mpk::dss::core
{

struct TrackPoint
{
    std::optional<QGeoCoordinate> coordinate;
    std::optional<Direction> direction;
    QDateTime timestamp;
    QJsonObject rawData;
};
using TrackPoints = std::vector<TrackPoint>;

struct Track
{
    TrackId id;
    TargetId targetId;
    DeviceId sourceId;
    TrackPoints points;
};

} // namespace mpk::dss::core
