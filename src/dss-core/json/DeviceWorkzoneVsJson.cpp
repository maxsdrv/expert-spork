#include "DeviceWorkzoneVsJson.h"

#include "DeviceWorkzoneSectorVsJson.h"

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<DeviceWorkzone>::convert(const DeviceWorkzone& data)
{
    auto array = QJsonArray{};
    std::transform(
        data.sectors.begin(),
        data.sectors.end(),
        std::back_inserter(array),
        [](const auto& sector) { return json::toValue(sector); });
    return array;
}

DeviceWorkzone FromJsonConverter<DeviceWorkzone>::get(const QJsonValue& value)
{
    auto array = value.toArray();
    auto sectors = decltype(DeviceWorkzone::sectors){};
    std::transform(
        array.begin(),
        array.end(),
        std::back_inserter(sectors),
        [](const auto& value)
        { return json::fromValue<decltype(sectors)::value_type>(value); });
    return {sectors};
}
