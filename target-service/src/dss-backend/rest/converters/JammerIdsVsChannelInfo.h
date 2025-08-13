#pragma once

#include "dss-backend/rest/channel/ChannelInfo.h"
#include "dss-core/DeviceId.h"

namespace mpk::dss::backend::rest
{

struct JammerIdsVsChannelInfo
{
    static std::vector<core::DeviceId> convert(const ChannelInfo&);
};

} // namespace mpk::dss::backend::rest
