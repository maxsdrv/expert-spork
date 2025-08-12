#pragma once

#include <string>
#include <vector>

namespace mpk::dss::imitator::tamerlan
{

struct JammerChannelParameter
{
    std::string name;
};
using JammerChannelsParameters = std::vector<JammerChannelParameter>;

} // namespace mpk::dss::imitator::tamerlan
