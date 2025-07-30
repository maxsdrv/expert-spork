#include "ChannelInfoApiAdapter.h"

#include "dss-backend/rest/json/ChannelInfoVsJson.h"

#include "mpk/rest/ApiGateway.h"

namespace mpk::dss
{

ChannelInfoApiAdapter::ChannelInfoApiAdapter(
    rest::ApiGateway& apiGateway,
    std::vector<backend::rest::SingleChannelInfo> channels) :
  m_channelInfo{std::move(channels)}
{
    registerHandlers(apiGateway);
}

void ChannelInfoApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    apiGateway.registerSyncHandler(
        "get_channel_info",
        [this]() { return json::toValue(m_channelInfo).toObject(); });
}

} // namespace mpk::dss
