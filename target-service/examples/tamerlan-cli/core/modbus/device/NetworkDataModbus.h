#pragma once

#include "core/device/NetworkData.h"
#include "core/proto/Common.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

#pragma pack(push, 2)

struct NetworkDataModbus
{
    uint32_t networkAddress = 0;
    uint32_t netmask = 0;
    uint32_t gateway = 0;
    uint32_t dns = 0;
    uint16_t flags = 0;

    static void toRegs(
        const NetworkDataModbus& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = uint32ToRegs(data.networkAddress, it);
        it = uint32ToRegs(data.netmask, it);
        it = uint32ToRegs(data.gateway, it);
        it = uint32ToRegs(data.dns, it);
        *it = data.flags;
    }

    template <typename ConstIterator>
    static NetworkDataModbus fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(NetworkDataModbus) / 2);

        NetworkDataModbus result;
        auto it = begin;
        std::tie(result.networkAddress, it) = regsToUint32(it);
        std::tie(result.netmask, it) = regsToUint32(it);
        std::tie(result.gateway, it) = regsToUint32(it);
        std::tie(result.dns, it) = regsToUint32(it);
        result.flags = *(it++);
        return result;
    }
};

#pragma pack(pop)

class ModbusData;

void connectNetworkDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<NetworkDataHolder*> holder);

} // namespace mpk::drone::core
