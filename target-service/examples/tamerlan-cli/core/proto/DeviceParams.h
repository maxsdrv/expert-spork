#pragma once

#include "core/proto/Common.h"
#include "core/proto/Protocol.h"

#include "gsl/assert"

namespace mpk::drone::core
{

#pragma pack(push, 2)

struct DeviceParamsModbus
{
    float minVoltage = 0.0F;
    float maxVoltage = 0.0F;
    float maxTemperature = 0.0F;
    float channelsDelay = 0.0F;
    float maxTimeForCurrent = 0.0F;
    float maxTimeForVoltage = 0.0F;

    static void toRegs(
        const DeviceParamsModbus& data, U16BackInsertIterator auto begin)
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
    static DeviceParamsModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(DeviceParamsModbus) / 2);

        DeviceParamsModbus result;
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
    sizeof(DeviceParamsModbus)
        == 2 * mpk::drone::core::proto::DeviceParams::size,
    "Size of JammerModbus is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::drone::core
