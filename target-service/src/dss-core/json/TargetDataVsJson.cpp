#include "TargetDataVsJson.h"

#include "dss-core/json/AlarmStatusVsJson.h"
#include "dss-core/json/TargetDataVsJson.h"
#include "dss-core/json/TargetPositionVsJson.h"

#include "types/strong_typedef/json/JsonComposer.hpp"

namespace
{

constexpr auto targetIdTag = "target_id";
constexpr auto sensorIdTag = "sensor_id";
constexpr auto trackIdTag = "track_id";
constexpr auto cameraTrackTag = "camera_track";
constexpr auto alarmIdsTag = "alarm_ids";
constexpr auto clsIdsTag = "cls_ids";
constexpr auto classNameTag = "class_name";
constexpr auto attributesTag = "attributes";
constexpr auto alarmStatusTag = "alarm_status";
constexpr auto positionTag = "position";
constexpr auto lastUpdatedTag = "last_updated";
constexpr auto detectCountTag = "detect_count";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<TargetData>::convert(const TargetData& data)
{
    auto obj = QJsonObject{
        {targetIdTag, json::toValue(data.id)},
        {sensorIdTag, json::toValue(data.sensorId)},
        {trackIdTag, json::toValue(data.trackId)},
        {alarmIdsTag, json::toValue(data.alarmIds)},
        {clsIdsTag, json::toValue(data.clsIds)},
        {classNameTag, json::toValue(data.className)},
        {attributesTag, json::toValue(data.attributes)},
        {alarmStatusTag, json::toValue(data.alarmStatus)},
        {positionTag, json::toValue(data.position)},
        {lastUpdatedTag, json::toValue(data.lastUpdated)},
        {detectCountTag, QJsonValue(data.detectCount)}};

    if (data.cameraTrack.has_value())
    {
        obj.insert(cameraTrackTag, data.cameraTrack->asJsonObject());
    }

    return obj;
}
