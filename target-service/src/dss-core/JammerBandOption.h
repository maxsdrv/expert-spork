#pragma once

#include "JammerBand.h"

#include <set>
#include <vector>

namespace mpk::dss::core
{

struct strFreqComp;

using JammerBandLabelSet = std::set<JammerLabel, strFreqComp>;
using JammerBandOption = std::vector<JammerLabel>;
using JammerBandOptionVec = std::vector<JammerBandOption>;

const auto JAMMER_BANDS_OFF = JammerBandOption();

template <typename BandOption>
JammerBands toBands(BandOption bandOpt, bool active)
{
    JammerBands bands;
    std::ranges::transform(
        bandOpt,
        std::back_inserter(bands),
        [active](const auto& label) {
            return core::JammerBand{label, active};
        });
    return bands;
}

JammerBandOption toBandOptionFull(
    JammerBands::const_iterator first, JammerBands::const_iterator last);
JammerBandOption toBandOptionFilter(JammerBands bands, bool takeActive = true);

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::JammerBandOption);
