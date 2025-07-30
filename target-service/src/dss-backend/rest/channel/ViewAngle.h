#pragma once

namespace mpk::dss::backend::rest
{

struct ViewAngle
{
    static constexpr auto defaultMin = 0.;
    static constexpr auto defaultMax = 360.;

    double min = defaultMin;
    double max = defaultMax;
};

} // namespace mpk::dss::backend::rest
