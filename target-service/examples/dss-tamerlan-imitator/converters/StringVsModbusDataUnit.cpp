#include "StringVsModbusDataUnit.h"

namespace mpk::dss::imitator::tamerlan
{

void StringVsModbusDataUnit::store(
    const std::string& string, int maxSize, QModbusDataUnit& unit, int index)
{
    using regValueType = quint16;

    auto strLength = std::min(string.length(), static_cast<size_t>(maxSize));
    std::string stringToWriting(string, 0, strLength);
    QByteArray data{stringToWriting.data()};
    if (data.length() < maxSize)
    {
        data.append('\0');
    }
    QDataStream dataStream{data};
    dataStream.setByteOrder(QDataStream::LittleEndian);
    auto address{index};
    while (!dataStream.atEnd() && (dataStream.device()->pos() < maxSize))
    {
        regValueType value{0};
        dataStream >> value;
        unit.setValue(address++, value);
    }
}

} // namespace mpk::dss::imitator::tamerlan
