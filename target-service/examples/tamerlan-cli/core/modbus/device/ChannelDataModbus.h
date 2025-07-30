#pragma once

#include "core/device/ChannelData.h"
#include "core/proto/Common.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

struct ChannelZoneModbus
{
    float direction = 0.0F;
    float workAngle = 0.0F;
    float distance = 0.0F;

    static void toRegs(
        const ChannelZoneModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = floatToRegs(data.direction, it);
        it = floatToRegs(data.workAngle, it);
        it = floatToRegs(data.distance, it);
    }

    template <typename ConstIterator>
    static ChannelZoneModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(ChannelZoneModbus) / 2);

        ChannelZoneModbus result;
        auto it = begin;
        std::tie(result.direction, it) = regsToFloat(it);
        std::tie(result.workAngle, it) = regsToFloat(it);
        std::tie(result.distance, it) = regsToFloat(it);
        return result;
    }
};

struct ChannelParamsModbus
{
    int32_t samplingFrequency = 0;
    int32_t bandwidth = 0;

    static void toRegs(
        const ChannelParamsModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = int32ToRegs(data.samplingFrequency, it);
        it = int32ToRegs(data.bandwidth, it);
    }

    template <typename ConstIterator>
    static ChannelParamsModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(ChannelParamsModbus) / 2);

        ChannelParamsModbus result;
        auto it = begin;
        std::tie(result.samplingFrequency, it) = regsToInt32(it);
        std::tie(result.bandwidth, it) = regsToInt32(it);
        return result;
    }
};

class ModbusData;

void connectChannelDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<ChannelDataHolder*> holder);

} // namespace mpk::drone::core
