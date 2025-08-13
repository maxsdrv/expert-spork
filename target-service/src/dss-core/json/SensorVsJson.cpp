#include "SensorVsJson.h"

#include "DeviceHwInfoVsJson.h"
#include "DevicePositionVsJson.h"
#include "DeviceWorkzoneSectorVsJson.h"
#include "SensorProperties.h"

#include "types/strong_typedef/json/JsonComposer.hpp"

using namespace mpk::dss::core;

namespace
{

constexpr auto idTag = "id";
constexpr auto typeTag = "type";
constexpr auto modelTag = "model";
constexpr auto serialTag = "serial";
constexpr auto swVersionTag = "sw_version";
constexpr auto disabledTag = "disabled";
constexpr auto positionTag = "position";
constexpr auto positionModeTag = "position_mode";
constexpr auto workzoneTag = "workzone";
constexpr auto stateTag = "state";
constexpr auto jammerIdsTag = "jammer_ids";
constexpr auto jammerModeTag = "jammer_mode";
constexpr auto jammerTimeoutTag = "jammer_auto_timeout";
constexpr auto hwInfoTag = "hw_info";

} // namespace

QJsonValue ToJsonConverter<SensorDevice>::convert(const SensorDevice& sensor)
{
    std::vector<QJsonObject> sectorsJson;
    auto sectors = sensor.properties().workzone().sectors;
    std::transform(
        sectors.begin(),
        sectors.end(),
        std::back_inserter(sectorsJson),
        [](const auto& sectorIt)
        { return json::toValue(sectorIt).toObject(); });

    OAService::OAServiceGeo_position_mode positionMode;
    positionMode.setValue(sensor.properties().positionMode());

    return QJsonObject{
        {idTag, json::toValue(sensor.id())},
        {typeTag, DeviceType::toString(sensor.properties().type())},
        {modelTag, sensor.properties().model()},
        {serialTag, sensor.properties().serial()},
        {swVersionTag, sensor.properties().swVersion()},
        {disabledTag, sensor.properties().disabled()},
        {stateTag, DeviceState::toString(sensor.properties().state())},
        {positionTag, json::toValue(sensor.properties().position())},
        {positionModeTag, positionMode.asJson()},
        {workzoneTag, json::toValue(sectorsJson)},
        {jammerIdsTag, json::toValue(sensor.properties().jammerIds())},
        {jammerModeTag, JammerMode::toString(sensor.properties().jammerMode())},
        {jammerTimeoutTag,
         static_cast<int>(sensor.properties().jammerAutoTimeout().count())},
        {hwInfoTag, json::toValue(sensor.properties().hwInfo())},
    };
}
