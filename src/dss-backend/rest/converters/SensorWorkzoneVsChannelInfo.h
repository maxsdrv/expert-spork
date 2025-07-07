#pragma once

#include "dss-backend/rest/channel/ChannelInfo.h"
#include "dss-core/DeviceWorkzone.h"

namespace mpk::dss::backend::rest
{

struct DeviceWorkzoneVsChannelInfo
{
    static core::DeviceWorkzone convert(const ChannelInfo& channelInfo);
};

} // namespace mpk::dss::backend::rest
