#pragma once

#include "dss-backend/tamerlan/modbus/Common.h"

#include <array>
#include <ostream>
#include <tuple>

namespace mpk::dss::backend::tamerlan
{

struct DeviceInfo
{
    static constexpr auto modelMaxSize = 64;
    std::array<char, modelMaxSize> model = {};
    static constexpr auto serialMaxSize = 16;
    std::array<char, serialMaxSize> serial = {};
    uint32_t version = 0;
    uint64_t revision = 0;
    uint32_t updateDate = 0;

    static void toRegs(const DeviceInfo& data, U16BackInsertIterator auto begin);

    template <typename ConstIterator>
    static DeviceInfo fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(DeviceInfo) / 2);

        DeviceInfo result;
        auto it = begin;
        it = regsToCharArray(result.model.data(), result.model.size(), it);
        it = regsToCharArray(result.serial.data(), result.serial.size(), it);
        std::tie(result.version, it) = regsToUint32(it);
        std::tie(result.revision, it) = regsToUint64(it);
        std::tie(result.updateDate, it) = regsToUint32(it);
        return result;
    }

    static std::ostream& streamSwVersion(
        std::ostream& stream, const DeviceInfo& deviceInfo);
};

} // namespace mpk::dss::backend::tamerlan
