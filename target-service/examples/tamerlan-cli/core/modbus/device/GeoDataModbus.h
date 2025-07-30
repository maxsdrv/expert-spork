#pragma once

#include "core/device/GeoData.h"
#include "core/proto/Common.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

struct GeoDataModbus
{
    float latitude = 0.0F;
    float longitude = 0.0F;
    float orientation = 0.0F;

    static void toRegs(
        const GeoDataModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.latitude, it);
        it = floatToRegs(data.longitude, it);
        it = floatToRegs(data.orientation, it);
    }

    template <typename ConstIterator>
    static GeoDataModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(GeoDataModbus) / 2);

        GeoDataModbus result;
        auto it = begin;
        std::tie(result.latitude, it) = regsToFloat(it);
        std::tie(result.longitude, it) = regsToFloat(it);
        std::tie(result.orientation, it) = regsToFloat(it);
        return result;
    }
};

class ModbusData;

void connectGeoDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<GeoDataHolder*> holder);

} // namespace mpk::drone::core
