#pragma once

#include "dss-backend/rest/channel/SingleChannelInfo.h"

#include <vector>

namespace mpk::dss::backend::rest
{

struct ChannelInfo
{
    std::vector<SingleChannelInfo> objects;
};

} // namespace mpk::dss::backend::rest
