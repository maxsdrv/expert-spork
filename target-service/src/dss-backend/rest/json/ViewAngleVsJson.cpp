#include "dss-backend/rest/json/ViewAngleVsJson.h"

namespace
{

constexpr auto minTag = "min";
constexpr auto maxTag = "max";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<ViewAngle>::convert(const ViewAngle& data)
{
    return QJsonObject{
        {minTag, json::toValue(data.min)}, {maxTag, json::toValue(data.max)}};
}

ViewAngle FromJsonConverter<ViewAngle>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .min = json::fromObject<decltype(ViewAngle::min)>(object, minTag),
        .max = json::fromObject<decltype(ViewAngle::max)>(object, maxTag)};
}
