#pragma once

#include "Common.h"

#include "gsl/pointers"

namespace mpk::dss::backend::tamerlan
{

#pragma pack(push, 2)

struct NetworkData
{
    uint32_t networkAddress = 0;
    uint32_t netmask = 0;
    uint32_t gateway = 0;
    uint32_t dns = 0;
    uint16_t flags = 0;

    static void toRegs(const NetworkData& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = uint32ToRegs(data.networkAddress, it);
        it = uint32ToRegs(data.netmask, it);
        it = uint32ToRegs(data.gateway, it);
        it = uint32ToRegs(data.dns, it);
        *it = data.flags;
    }

    template <typename ConstIterator>
    static NetworkData fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(NetworkData) / 2);

        NetworkData result;
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

} // namespace mpk::dss::backend::tamerlan
