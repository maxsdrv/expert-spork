#include "JammerBand.h"

namespace mpk::dss::core
{

bool operator==(const JammerBand& left, const JammerBand& right)
{
    return std::tie(left.label, left.active)
           == std::tie(right.label, right.active);
}

bool operator!=(const JammerBand& left, const JammerBand& right)
{
    return !(left == right);
}

JammerBands setAll(JammerBands bands, bool value)
{
    for (JammerBand& b: bands)
    {
        b.active = value;
    }
    return bands;
}

} // namespace mpk::dss::core
