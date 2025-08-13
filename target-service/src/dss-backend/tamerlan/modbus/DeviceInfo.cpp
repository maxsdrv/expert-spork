#include "DeviceInfo.h"

namespace mpk::dss::backend::tamerlan
{

void DeviceInfo::toRegs(const DeviceInfo& data, U16BackInsertIterator auto begin)
{
    auto it = begin;
    it = charArrayToRegs(data.model.data(), data.model.size(), it);
    it = charArrayToRegs(data.serial.data(), data.serial.size(), it);
    it = uint32ToRegs(data.version, it);
    it = uint64ToRegs(data.revision, it);
    it = uint32ToRegs(data.updateDate, it);
}

std::ostream& DeviceInfo::streamSwVersion(
    std::ostream& stream, const DeviceInfo& deviceInfo)
{
    constexpr auto twentyFourBit = sixteenBit + eightBit;
    auto major = (deviceInfo.version >> twentyFourBit) & eightBitMask;
    auto minor = (deviceInfo.version >> sixteenBit) & eightBitMask;
    auto version = (deviceInfo.version >> eightBit) & eightBitMask;
    auto build = deviceInfo.version & eightBitMask;

    auto year = (deviceInfo.updateDate >> sixteenBit) & sixteenBitMask;
    auto month = (deviceInfo.updateDate >> eightBit) & eightBitMask;
    auto day = deviceInfo.updateDate & eightBitMask;

    stream << major << "." << minor << "." << version << "." << build;
    stream << "(" << std::hex << deviceInfo.version << std::dec << ")";
    stream << " [" << year << "-" << month << "-" << day << "]";

    return stream;
}

} // namespace mpk::dss::backend::tamerlan
