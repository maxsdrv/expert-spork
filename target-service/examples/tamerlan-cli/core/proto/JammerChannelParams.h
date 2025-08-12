#pragma once

#include "core/proto/Common.h"
#include "core/proto/Protocol.h"

#include "gsl/assert"

namespace mpk::drone::core
{

#pragma pack(push, 2)

struct JammerChannelParamsModbus
{
    float maxCurrent = 0.0F;
    float maxImpulseCurrent = 0.0F;

    static void toRegs(
        const JammerChannelParamsModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.maxCurrent, it);
        it = floatToRegs(data.maxImpulseCurrent, it);
    }

    template <typename ConstIterator>
    static JammerChannelParamsModbus fromRegs(
        ConstIterator begin, ConstIterator end)
    {
        Expects(
            std::distance(begin, end) == sizeof(JammerChannelParamsModbus) / 2);

        JammerChannelParamsModbus result;
        auto it = begin;
        std::tie(result.maxCurrent, it) = regsToFloat(it);
        std::tie(result.maxImpulseCurrent, it) = regsToFloat(it);
        return result;
    }
};

#pragma pack(pop)

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(
    sizeof(JammerChannelParamsModbus)
        == 2 * mpk::drone::core::proto::JammerChannelParams::size,
    "Size of JammerModbus is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::drone::core
