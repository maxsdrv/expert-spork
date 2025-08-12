#pragma once

#include <QModbusDataUnit>

namespace mpk::dss::imitator::tamerlan
{

struct DeviceInfoModbus
{
    static const auto modelMaxSize = 64;
    std::string model = {};
    static const auto serialMaxSize = 16;
    std::string serial = {};
    uint32_t version = 0;
    uint64_t revision = 0;
    uint32_t updateDate = 0;

    void store(QModbusDataUnit& unit) const;
};

} // namespace mpk::dss::imitator::tamerlan
