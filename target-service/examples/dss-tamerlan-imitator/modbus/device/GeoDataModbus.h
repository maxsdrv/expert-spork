#pragma once

#include <QModbusDataUnit>

namespace mpk::dss::imitator::tamerlan
{

struct GeoDataModbus
{
    float latitude = 0.0F;
    float longitude = 0.0F;
    float orientation = 0.0F;

    void store(QModbusDataUnit& unit) const;
};

} // namespace mpk::dss::imitator::tamerlan
