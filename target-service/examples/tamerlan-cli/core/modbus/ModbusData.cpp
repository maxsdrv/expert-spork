#include "ModbusData.h"

#include "core/proto/Protocol.h"

#include <boost/numeric/interval.hpp>

namespace mpk::drone::core
{

ModbusData::ModbusData(
    gsl::not_null<ModbusServerController*> controller, QObject* parent) :
  QObject(parent), m_controller(controller)
{
    // TODO: Made proper register map
    // NOLINTBEGIN
    // (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    controller->server()->setMap(
        {{QModbusDataUnit::HoldingRegisters,
          {QModbusDataUnit::HoldingRegisters, 0, 0xFFFF}},
         {QModbusDataUnit::InputRegisters,
          {QModbusDataUnit::InputRegisters, 0, 0xFFFF}},
         {QModbusDataUnit::Coils, {QModbusDataUnit::Coils, 0, 0xFFFF}}});
    // NOLINTEND
    // (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    QObject::connect(
        m_controller->server(),
        &QModbusServer::dataWritten,
        this,
        [this](auto /*table*/, auto address, auto size)
        {
            const boost::numeric::interval<int> modifiedInterval(
                address, address + size - 1);
            for (const auto& record: m_records)
            {
                const boost::numeric::interval<int> recordInterval(
                    record.second.address,
                    record.second.address + record.second.size - 1);
                if (boost::numeric::overlap(modifiedInterval, recordInterval))
                {
                    emit modified(record.first);
                }
            }
        });
}

} // namespace mpk::drone::core
