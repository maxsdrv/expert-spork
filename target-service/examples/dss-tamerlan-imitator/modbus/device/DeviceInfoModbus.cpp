#include "DeviceInfoModbus.h"

#include "converters/StringVsModbusDataUnit.h"
#include "modbus/Common.h"

namespace mpk::dss::imitator::tamerlan
{

namespace
{
constexpr auto serialIndex = 0x20;
constexpr auto versionIndex = 0x28;
constexpr auto revisionIndex = 0x2a;
constexpr auto updateDateIndex = 0x2e;
} // namespace

void DeviceInfoModbus::store(QModbusDataUnit& unit) const
{
    StringVsModbusDataUnit::store(model, modelMaxSize, unit);
    StringVsModbusDataUnit::store(serial, serialMaxSize, unit, serialIndex);
    valueToRegs<uint32_t>(version, unit, versionIndex);
    valueToRegs<uint64_t>(revision, unit, revisionIndex);
    valueToRegs<uint32_t>(updateDate, unit, updateDateIndex);
}

} // namespace mpk::dss::imitator::tamerlan
