#include "ModbusParamsModbus.h"

#include "core/modbus/ModbusData.h"
#include "core/proto/Protocol.h"

namespace mpk::drone::core
{

void connectModbusParamsWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<ModbusParamsHolder*> holder)
{
    modbusData->emplace<uint16_t>(
        proto::ModbusParams::id,
        proto::ModbusParams::address,
        proto::ModbusParams::type,
        static_cast<uint16_t>(holder->params().address));
    QObject::connect(
        modbusData,
        &ModbusData::modified,
        holder,
        [modbusData, holder](const auto& id)
        {
            if (id == proto::ModbusParams::id)
            {
                uint16_t address = 0U;
                modbusData->read(
                    proto::ModbusParams::id,
                    address,
                    proto::ModbusParams::type);
                holder->setParams({address});
            }
        });
    QObject::connect(
        holder,
        &ModbusParamsHolder::changed,
        modbusData,
        [modbusData](const auto& params)
        {
            modbusData->write(
                proto::ModbusParams::id,
                static_cast<uint16_t>(params.address),
                proto::ModbusParams::type);
        });
}

} // namespace mpk::drone::core
