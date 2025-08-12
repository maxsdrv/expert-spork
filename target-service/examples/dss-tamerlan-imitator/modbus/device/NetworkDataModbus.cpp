#include "NetworkDataModbus.h"

#include <QDataStream>

namespace mpk::dss::imitator::tamerlan
{

void mpk::dss::imitator::tamerlan::NetworkDataModbus::store(
    QModbusDataUnit& unit) const
{
    using regValueType = quint16;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    QByteArray data{
        reinterpret_cast<const char*>(this), sizeof(NetworkDataModbus)};
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    QDataStream dataStream{data};
    dataStream.setByteOrder(QDataStream::LittleEndian);
    auto index{0};
    while (!dataStream.atEnd() && dataStream.device()->pos() < data.size())
    {
        regValueType value{0};
        dataStream >> value;
        unit.setValue(index++, value);
    }
}

} // namespace mpk::dss::imitator::tamerlan
