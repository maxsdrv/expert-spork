#pragma once

#include <QDataStream>
#include <QModbusDataUnit>

namespace mpk::dss::imitator::tamerlan
{

struct StringVsModbusDataUnit
{
    static void store(
        const std::string& string,
        int maxSize,
        QModbusDataUnit& unit,
        int index = 0);
};

} // namespace mpk::dss::imitator::tamerlan
