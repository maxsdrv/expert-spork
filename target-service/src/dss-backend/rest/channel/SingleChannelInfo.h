#pragma once

#include "dss-backend/rest/channel/ViewAngle.h"

#include <QString>

#include <vector>

namespace mpk::dss::backend::rest
{

using JammerIds = std::vector<QString>;

struct SingleChannelInfo
{
    static constexpr auto defaultRange = 3000.;

    double range = defaultRange;
    ViewAngle viewAngle;
    JammerIds jammerIds;
};

} // namespace mpk::dss::backend::rest
