#include "DevicePositionVsJson.h"

#include "dss-core/json/QtGeoJson.h"

namespace
{

constexpr auto azimuthTag = "azimuth";
constexpr auto coordinateTag = "coordinate";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<DevicePosition>::convert(
    const mpk::dss::core::DevicePosition& position)
{
    return QJsonObject{
        {azimuthTag, json::toValue(position.azimuth)},
        {coordinateTag, json::toValue(position.coordinate)}};
}

DevicePosition FromJsonConverter<DevicePosition>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .azimuth = json::fromObject<decltype(DevicePosition::azimuth)>(
            object, azimuthTag),
        .coordinate = json::fromObject<decltype(DevicePosition::coordinate)>(
            object, coordinateTag)};
}
