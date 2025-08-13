#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <QDateTime>
#include <QGeoCoordinate>

namespace mpk::dss::backend::rest
{

using Frequency = std::int64_t;

struct SensorTargetData
{
    std::string type;
    int id;
    int channel;
    std::string description;
    std::optional<double> bearing;
    std::optional<double> distance;
    std::optional<QGeoCoordinate> coordinate;
    std::vector<Frequency> frequencies;
    std::optional<int> blocksCount;
    QDateTime timestamp;

    static constexpr auto defaultType = "target_data";
};

} // namespace mpk::dss::backend::rest
