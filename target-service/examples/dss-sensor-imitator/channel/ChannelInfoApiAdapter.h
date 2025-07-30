#pragma once

#include "dss-backend/rest/channel/ChannelInfo.h"

#include <vector>

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss
{

class ChannelInfoApiAdapter
{
public:
    ChannelInfoApiAdapter(
        rest::ApiGateway& apiGateway,
        std::vector<backend::rest::SingleChannelInfo> channels);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    backend::rest::ChannelInfo m_channelInfo;
};

} // namespace mpk::dss
