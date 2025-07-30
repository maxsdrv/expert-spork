#pragma once

#include <QModbusDataUnit>

#include <cstdint>

namespace mpk::dss::imitator::tamerlan
{

#pragma pack(push, 2)

struct NetworkDataModbus
{
    uint32_t networkAddress = 0;
    uint32_t netmask = 0;
    uint32_t gateway = 0;
    uint32_t dns = 0;
    uint16_t flags = 0;

    void store(QModbusDataUnit& unit) const;
};

#pragma pack(pop)

} // namespace mpk::dss::imitator::tamerlan
