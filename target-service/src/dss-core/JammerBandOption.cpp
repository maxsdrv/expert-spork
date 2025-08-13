#include "JammerBandOption.h"

#include <algorithm>
#include <iterator>

namespace mpk::dss::core
{

JammerBandOption toBandOptionFull(
    JammerBands::const_iterator first, JammerBands::const_iterator last)
{
    JammerBandOption bandOpt;
    std::transform(
        first,
        last,
        std::inserter(bandOpt, bandOpt.begin()),
        [](const JammerBand& b) { return b.label; });
    return bandOpt;
}

JammerBandOption toBandOptionFilter(JammerBands bands, bool takeActive)
{
    const bool removeActive = !takeActive;
    auto filtered = remove_if(
        bands.begin(),
        bands.end(),
        [&](const JammerBand& b) { return b.active == removeActive; });

    return toBandOptionFull(bands.begin(), filtered);
}

} // namespace mpk::dss::core
