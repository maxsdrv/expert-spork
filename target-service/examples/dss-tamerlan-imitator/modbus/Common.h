#pragma once

#include <QDataStream>
#include <QModbusDataUnit>

#include <cstdint>

namespace mpk::dss::imitator::tamerlan
{

template <typename T>
void valueToRegs(T value, QModbusDataUnit& unit, int index = 0)
{
    using regValueType = quint16;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    QByteArray data{reinterpret_cast<const char*>(&value), sizeof(value)};
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    QDataStream dataStream{data};
    dataStream.setByteOrder(QDataStream::LittleEndian);
    auto address{index};
    while (!dataStream.atEnd() && dataStream.device()->pos() < data.size())
    {
        regValueType value{0};
        dataStream >> value;
        unit.setValue(address++, value);
    }
}

} // namespace mpk::dss::imitator::tamerlan
