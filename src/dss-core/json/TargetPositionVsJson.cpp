#include "dss-core/json/TargetPositionVsJson.h"

#include "dss-core/json/TargetDirectionVsJson.h"

#include "types/strong_typedef/json/JsonComposer.hpp"
#include "types/strong_typedef/json/JsonParser.hpp"

#include "dss-core/json/QtGeoJson.h"

namespace
{

constexpr auto coordinateTag = "coordinate";
constexpr auto directionTag = "direction";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<TargetPosition>::convert(
    const TargetPosition& position)
{
    QJsonObject obj;
    if (position.coordinate.has_value())
    {
        obj.insert(coordinateTag, json::toValue(position.coordinate));
    }
    if (position.direction.has_value())
    {
        obj.insert(directionTag, json::toValue(position.direction));
    }
    return obj;
}

TargetPosition FromJsonConverter<TargetPosition>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .coordinate = json::fromObject<decltype(TargetPosition::coordinate)>(
            object, coordinateTag),
        .direction = json::fromObject<decltype(TargetPosition::direction)>(
            object, directionTag)};
}
