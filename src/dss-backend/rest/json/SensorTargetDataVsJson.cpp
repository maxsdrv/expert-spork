#include "SensorTargetDataVsJson.h"

#include "dss-core/json/QtGeoJson.h"

namespace
{

constexpr auto typeTag = "type";
constexpr auto idTag = "id";
constexpr auto channelTag = "channel";
constexpr auto descriptionTag = "description";
constexpr auto bearingTag = "bearing";
constexpr auto distanceTag = "distance";
constexpr auto coordinatesTag = "coordinate";
constexpr auto frequenciesTag = "frequencies";
constexpr auto blocksCountTag = "blocks_count";
constexpr auto timestampTag = "timestamp";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<SensorTargetData>::convert(
    const SensorTargetData& targetData)
{
    return QJsonObject{
        {typeTag, json::toValue(targetData.type)},
        {idTag, json::toValue(targetData.id)},
        {channelTag, json::toValue(targetData.channel)},
        {descriptionTag, json::toValue(targetData.description)},
        {bearingTag, json::toValue(targetData.bearing)},
        {distanceTag, json::toValue(targetData.distance)},
        {coordinatesTag, json::toValue(targetData.coordinate)},
        {frequenciesTag, json::toValue(targetData.frequencies)},
        {blocksCountTag, json::toValue(targetData.blocksCount)},
        {timestampTag, json::toValue(targetData.timestamp)}};
}

SensorTargetData FromJsonConverter<SensorTargetData>::get(
    const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .type =
            json::fromObject<decltype(SensorTargetData::type)>(object, typeTag),
        .id = json::fromObject<decltype(SensorTargetData::id)>(object, idTag),
        .channel = json::fromObject<decltype(SensorTargetData::channel)>(
            object, channelTag),
        .description =
            json::fromObject<decltype(SensorTargetData::description)>(
                object, descriptionTag),
        .bearing = json::fromObject<decltype(SensorTargetData::bearing)>(
            object, bearingTag),
        .distance = json::fromObject<decltype(SensorTargetData::distance)>(
            object, distanceTag),
        .coordinate = json::fromObject<decltype(SensorTargetData::coordinate)>(
            object, coordinatesTag),
        .frequencies =
            json::fromObject<decltype(SensorTargetData::frequencies)>(
                object, frequenciesTag),
        .blocksCount =
            json::fromObject<decltype(SensorTargetData::blocksCount)>(
                object, blocksCountTag),
        .timestamp = json::fromObject<decltype(SensorTargetData::timestamp)>(
            object, timestampTag)};
}
