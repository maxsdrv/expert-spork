#pragma once

#include "Common.h"

#include "gsl/assert"

#include <tuple>

namespace mpk::dss::backend::tamerlan
{

struct ChannelZone
{
    float direction = 0.0F;
    float workAngle = 0.0F;
    float distance = 0.0F;

    static void toRegs(const ChannelZone& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.direction, it);
        it = floatToRegs(data.workAngle, it);
        it = floatToRegs(data.distance, it);
    }

    template <typename ConstIterator>
    static ChannelZone fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(ChannelZone) / 2);

        ChannelZone result;
        auto it = begin;
        std::tie(result.direction, it) = regsToFloat(it);
        std::tie(result.workAngle, it) = regsToFloat(it);
        std::tie(result.distance, it) = regsToFloat(it);
        return result;
    }
};

struct ChannelParams
{
    int32_t samplingFrequency = 0;
    int32_t bandwidth = 0;

    static void toRegs(
        const ChannelParams& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = int32ToRegs(data.samplingFrequency, it);
        it = int32ToRegs(data.bandwidth, it);
    }

    template <typename ConstIterator>
    static ChannelParams fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(ChannelParams) / 2);

        ChannelParams result;
        auto it = begin;
        std::tie(result.samplingFrequency, it) = regsToInt32(it);
        std::tie(result.bandwidth, it) = regsToInt32(it);
        return result;
    }
};

} // namespace mpk::dss::backend::tamerlan
