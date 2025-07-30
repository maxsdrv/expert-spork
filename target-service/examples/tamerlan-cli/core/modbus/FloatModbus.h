#pragma once

#include "core/proto/Common.h"

namespace mpk::drone::core
{

struct FloatModbus
{
    float value = 0.0F;

    static void toRegs(const FloatModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.value, it);
    }

    template <typename ConstIterator>
    static FloatModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(FloatModbus) / 2);

        FloatModbus result;
        auto it = begin;
        std::tie(result.value, it) = regsToFloat(it);
        return result;
    }
};

} // namespace mpk::drone::core
