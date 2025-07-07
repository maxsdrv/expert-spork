#pragma once

#include "Common.h"
#include "Protocol.h"

#include "gsl/assert"

#include <tuple>

namespace mpk::dss::backend::tamerlan
{

#pragma pack(push, 2)

struct JammerChannelParams
{
    float maxCurrent = 0.0F;
    float maxImpulseCurrent = 0.0F;

    static void toRegs(
        const JammerChannelParams& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.maxCurrent, it);
        it = floatToRegs(data.maxImpulseCurrent, it);
    }

    template <typename ConstIterator>
    static JammerChannelParams fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(JammerChannelParams) / 2);

        JammerChannelParams result;
        auto it = begin;
        std::tie(result.maxCurrent, it) = regsToFloat(it);
        std::tie(result.maxImpulseCurrent, it) = regsToFloat(it);
        return result;
    }
};

#pragma pack(pop)

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(
    sizeof(JammerChannelParams)
        == 2 * mpk::dss::backend::tamerlan::proto::JammerChannelParams::size,
    "Size of JammerModbus is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::dss::backend::tamerlan
