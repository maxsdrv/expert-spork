#include "JammerIdsVsChannelInfo.h"

namespace mpk::dss::backend::rest
{

std::vector<core::DeviceId> JammerIdsVsChannelInfo::convert(
    const ChannelInfo& channelInfo)
{
    std::vector<core::DeviceId> jammerIds;

    for (const auto& channel: channelInfo.objects)
    {
        for (const auto& id: channel.jammerIds)
        {
            jammerIds.emplace_back(id);
        }
    }

    return jammerIds;
}

} // namespace mpk::dss::backend::rest
