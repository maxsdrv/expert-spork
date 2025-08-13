#pragma once

#include "Common.h"

#include "gsl/assert"

#include <tuple>

namespace mpk::dss::backend::tamerlan
{

struct GeoData
{
    float latitude = 0.0F;
    float longitude = 0.0F;
    float orientation = 0.0F;

    static void toRegs(const GeoData& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.latitude, it);
        it = floatToRegs(data.longitude, it);
        it = floatToRegs(data.orientation, it);
    }

    template <typename ConstIterator>
    static GeoData fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(GeoData) / 2);

        GeoData result;
        auto it = begin;
        std::tie(result.latitude, it) = regsToFloat(it);
        std::tie(result.longitude, it) = regsToFloat(it);
        std::tie(result.orientation, it) = regsToFloat(it);
        return result;
    }
};

} // namespace mpk::dss::backend::tamerlan
