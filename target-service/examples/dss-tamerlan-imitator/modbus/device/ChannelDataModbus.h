#pragma once

#include <QModbusDataUnit>

namespace mpk::dss::imitator::tamerlan
{

struct ChannelZoneModbus
{
    float direction = 0.0F;
    float workAngle = 0.0F;
    float distance = 0.0F;

    void store(QModbusDataUnit& unit) const;
};

} // namespace mpk::dss::imitator::tamerlan
