#include "JammerInfoVsJson.h"

#include "DeviceHwInfoVsJson.h"
#include "DevicePositionVsJson.h"
#include "DeviceWorkzoneVsJson.h"
#include "dss-common/api/Helpers.h"

#include "types/strong_typedef/json/JsonComposer.hpp"
#include "types/strong_typedef/json/JsonParser.hpp"

#include <iomanip>

namespace
{

constexpr auto idTag = "id";
constexpr auto modelTag = "model";
constexpr auto serialTag = "serial";
constexpr auto swVersionTag = "sw_version";
constexpr auto positionTag = "position";
constexpr auto positionModeTag = "position_mode";
constexpr auto workzoneTag = "workzone";
constexpr auto disabledTag = "disabled";
constexpr auto stateTag = "state";
constexpr auto sensorIdTag = "sensor_id";
constexpr auto bandsTag = "bands";
constexpr auto bandOptionsTag = "band_options";
constexpr auto labelTag = "label";
constexpr auto activeTag = "active";
constexpr auto hwInfoTag = "hw_info";
constexpr auto startedTag = "started";
constexpr auto nowTag = "now";
constexpr auto durationTag = "duration";
constexpr auto statusTag = "timeout_status";
constexpr auto groupIdTag = "group_id";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<JammerBand>::convert(const JammerBand& band)
{
    return QJsonObject{
        {labelTag, json::toValue(band.label)},
        {activeTag, json::toValue(band.active)}};
}

JammerBand FromJsonConverter<JammerBand>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .label =
            json::fromObject<decltype(JammerBand::label)>(object, labelTag),
        .active =
            json::fromObject<decltype(JammerBand::active)>(object, activeTag)};
}

QJsonValue ToJsonConverter<JammerInfo>::convert(const JammerInfo& info)
{
    OAService::OAServiceGeo_position_mode positionMode;
    positionMode.setValue(info.positionMode);

    QJsonObject obj{
        {idTag, json::toValue(info.id)},
        {modelTag, json::toValue(info.model)},
        {serialTag, json::toValue(info.serial)},
        {swVersionTag, json::toValue(info.swVersion)},
        {positionTag, json::toValue(info.position)},
        {positionModeTag, json::toValue(positionMode.asJson())},
        {workzoneTag, json::toValue(info.workzone)},
        {disabledTag, json::toValue(info.disabled)},
        {stateTag, json::toValue(DeviceState::toString(info.state))},
        {sensorIdTag, json::toValue(info.sensorId)},
        {bandsTag, json::toValue(info.bands)},
        {hwInfoTag, json::toValue(info.hwInfo)}};
    if (!info.bandOptions.empty())
    {
        obj.insert(bandOptionsTag, json::toValue(info.bandOptions));
    }
    if (info.timeoutStatus.has_value())
    {
        obj.insert(statusTag, json::toValue(info.timeoutStatus));
    }
    if (info.groupId.has_value())
    {
        obj.insert(groupIdTag, json::toValue(info.groupId));
    }
    return obj;
}

QJsonValue ToJsonConverter<JammerTimeoutStatus>::convert(
    const JammerTimeoutStatus& status)
{
    return QJsonObject{
        {startedTag,
         json::toValue(mpk::dss::api::timeToStdString(status.started))},
        {nowTag, json::toValue(mpk::dss::api::timeToStdString(status.now))},
        {durationTag, status.timeoutSec}};
}
