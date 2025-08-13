#include "ChannelInfoVsJson.h"

#include "dss-backend/rest/json/SingleChannelInfoVsJson.h"

#include "log/Log2.h"

namespace
{

constexpr auto channelInfoTag = "channel_info";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<ChannelInfo>::convert(const ChannelInfo& data)
{
    return QJsonObject{{channelInfoTag, json::toValue(data.objects)}};
}

ChannelInfo FromJsonConverter<ChannelInfo>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .objects = json::fromObject<decltype(ChannelInfo::objects)>(
            object, channelInfoTag)};
}
