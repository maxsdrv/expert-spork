#include "DeviceWorkzoneSectorVsJson.h"

namespace
{

constexpr auto numberTag = "number";
constexpr auto distanceTag = "distance";
constexpr auto minAngleTag = "min_angle";
constexpr auto maxAngleTag = "max_angle";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<DeviceWorkzoneSector>::convert(
    const DeviceWorkzoneSector& data)
{
    return QJsonObject{
        {numberTag, json::toValue(data.number)},
        {distanceTag, json::toValue(data.distance)},
        {minAngleTag, json::toValue(data.minAngle)},
        {maxAngleTag, json::toValue(data.maxAngle)}};
}

DeviceWorkzoneSector FromJsonConverter<DeviceWorkzoneSector>::get(
    const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .number = json::fromObject<decltype(DeviceWorkzoneSector::number)>(
            object, numberTag),
        .distance = json::fromObject<decltype(DeviceWorkzoneSector::distance)>(
            object, distanceTag),
        .minAngle = json::fromObject<decltype(DeviceWorkzoneSector::minAngle)>(
            object, minAngleTag),
        .maxAngle = json::fromObject<decltype(DeviceWorkzoneSector::maxAngle)>(
            object, maxAngleTag)};
}
