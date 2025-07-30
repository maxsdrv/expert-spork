#include "GeoDataModbus.h"

#include "modbus/Common.h"

namespace mpk::dss::imitator::tamerlan
{

namespace
{
constexpr auto longitudeIndex = 0x2;
constexpr auto orientationIndex = 0x4;
} // namespace

void GeoDataModbus::store(QModbusDataUnit& unit) const
{
    valueToRegs<float>(latitude, unit);
    valueToRegs<float>(longitude, unit, longitudeIndex);
    valueToRegs<float>(orientation, unit, orientationIndex);
}

} // namespace mpk::dss::imitator::tamerlan
