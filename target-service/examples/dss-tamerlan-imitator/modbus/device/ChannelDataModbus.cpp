#include "ChannelDataModbus.h"

#include "modbus/Common.h"

namespace mpk::dss::imitator::tamerlan
{

namespace
{
constexpr auto workAngleIndex = 0x2;
constexpr auto distanceIndex = 0x4;
} // namespace

void ChannelZoneModbus::store(QModbusDataUnit& unit) const
{
    valueToRegs<float>(direction, unit);
    valueToRegs<float>(workAngle, unit, workAngleIndex);
    valueToRegs<float>(distance, unit, distanceIndex);
}

} // namespace mpk::dss::imitator::tamerlan
