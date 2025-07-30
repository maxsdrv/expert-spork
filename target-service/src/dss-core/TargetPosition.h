#pragma once

#include "dss-core/TargetDirection.h"

#include <QGeoCoordinate>

#include <optional>

namespace mpk::dss::core
{

struct TargetPosition
{
    std::optional<QGeoCoordinate> coordinate;
    std::optional<Direction> direction;
};

} // namespace mpk::dss::core
