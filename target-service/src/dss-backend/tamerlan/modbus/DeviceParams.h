#pragma once

#include "Common.h"
#include "Protocol.h"

#include "gsl/assert"

#include <tuple>

namespace mpk::dss::backend::tamerlan
{

#pragma pack(push, 2)

struct DeviceParams
{
    float minVoltage = 0.0F;
    float maxVoltage = 0.0F;
    float maxTemperature = 0.0F;
    float channelsDelay = 0.0F;
    float maxTimeForCurrent = 0.0F;
    float maxTimeForVoltage = 0.0F;

    static void toRegs(
        const DeviceParams& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.minVoltage, it);
        it = floatToRegs(data.maxVoltage, it);
        it = floatToRegs(data.maxTemperature, it);
        it = floatToRegs(data.channelsDelay, it);
        it = floatToRegs(data.maxTimeForCurrent, it);
        it = floatToRegs(data.maxTimeForVoltage, it);
    }

    template <typename ConstIterator>
    static DeviceParams fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(DeviceParams) / 2);

        DeviceParams result;
        auto it = begin;
        std::tie(result.minVoltage, it) = regsToFloat(it);
        std::tie(result.maxVoltage, it) = regsToFloat(it);
        std::tie(result.maxTemperature, it) = regsToFloat(it);
        std::tie(result.channelsDelay, it) = regsToFloat(it);
        std::tie(result.maxTimeForCurrent, it) = regsToFloat(it);
        std::tie(result.maxTimeForVoltage, it) = regsToFloat(it);
        return result;
    }
};

#pragma pack(pop)

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(
    sizeof(DeviceParams)
        == 2 * mpk::dss::backend::tamerlan::proto::DeviceParams::size,
    "Size of JammerModbus is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::dss::backend::tamerlan
