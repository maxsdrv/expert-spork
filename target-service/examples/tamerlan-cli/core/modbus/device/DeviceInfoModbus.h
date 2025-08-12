#pragma once

#include "core/device/DeviceInfo.h"
#include "core/proto/Common.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

struct DeviceInfoModbus
{
    static constexpr auto modelMaxSize = 64;
    std::array<char, modelMaxSize> model = {};
    static constexpr auto serialMaxSize = 16;
    std::array<char, serialMaxSize> serial = {};
    uint32_t version = 0;
    uint64_t revision = 0;
    uint32_t updateDate = 0;

    static void toRegs(
        const DeviceInfoModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = charArrayToRegs(data.model.data(), data.model.size(), it);
        it = charArrayToRegs(data.serial.data(), data.serial.size(), it);
        it = uint32ToRegs(data.version, it);
        it = uint64ToRegs(data.revision, it);
        it = uint32ToRegs(data.updateDate, it);
    }

    template <typename ConstIterator>
    static DeviceInfoModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(DeviceInfoModbus) / 2);

        DeviceInfoModbus result;
        auto it = begin;
        it = regsToCharArray(result.model.data(), result.model.size(), it);
        it = regsToCharArray(result.serial.data(), result.serial.size(), it);
        std::tie(result.version, it) = regsToUint32(it);
        std::tie(result.revision, it) = regsToUint64(it);
        std::tie(result.updateDate, it) = regsToUint32(it);
        return result;
    }
};

class ModbusData;

void connectDeviceInfoWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<DeviceInfoHolder*> holder);

} // namespace mpk::drone::core
