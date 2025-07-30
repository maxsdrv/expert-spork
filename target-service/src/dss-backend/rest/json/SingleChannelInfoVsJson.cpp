#include "SingleChannelInfoVsJson.h"

#include "dss-backend/rest/json/ViewAngleVsJson.h"

namespace
{

constexpr auto rangeTag = "range";
constexpr auto viewAngleTag = "view_angle";
constexpr auto jammerIdsTag = "jammer_ids";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<SingleChannelInfo>::convert(
    const SingleChannelInfo& data)
{
    return QJsonObject{
        {rangeTag, json::toValue(data.range)},
        {viewAngleTag, json::toValue(data.viewAngle)},
        {jammerIdsTag, json::toValue(data.jammerIds)}};
}

SingleChannelInfo FromJsonConverter<SingleChannelInfo>::get(
    const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .range = json::fromObject<decltype(SingleChannelInfo::range)>(
            object, rangeTag),
        .viewAngle =
            json::fromObjectWithDefault<decltype(SingleChannelInfo::viewAngle)>(
                object, viewAngleTag, {}),
        .jammerIds = json::fromObject<decltype(SingleChannelInfo::jammerIds)>(
            object, jammerIdsTag)};
}
