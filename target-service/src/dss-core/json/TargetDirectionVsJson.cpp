#include "dss-core/json/TargetDirectionVsJson.h"

namespace
{

constexpr auto bearingTag = "bearing";
constexpr auto distanceTag = "distance";
constexpr auto elevationTag = "elevation";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<Direction>::convert(const Direction& direction)
{
    return QJsonObject{
        {bearingTag, json::toValue(direction.bearing)},
        {distanceTag, json::toValue(direction.distance)},
        {elevationTag, json::toValue(direction.elevation)}};
}

Direction FromJsonConverter<Direction>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .bearing =
            json::fromObject<decltype(Direction::bearing)>(object, bearingTag),
        .distance =
            json::fromObject<decltype(Direction::distance)>(object, distanceTag),
        .elevation = json::fromObject<decltype(Direction::elevation)>(
            object, elevationTag)};
}
