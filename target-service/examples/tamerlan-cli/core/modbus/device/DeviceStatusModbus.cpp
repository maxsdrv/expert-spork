#include "DeviceStatusModbus.h"

#include "core/modbus/ModbusData.h"
#include "core/proto/Protocol.h"

namespace mpk::drone::core
{

void connectDeviceStatusWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<DeviceStatusHolder*> holder)
{
    modbusData->emplace<uint16_t>(
        proto::DeviceStatus::id,
        proto::DeviceStatus::address,
        proto::DeviceStatus::type,
        static_cast<uint16_t>(holder->status()));
    QObject::connect(
        modbusData,
        &ModbusData::modified,
        holder,
        [modbusData, holder](const auto& id)
        {
            if (id == proto::DeviceStatus::id)
            {
                uint16_t status = 0U;
                modbusData->read(
                    proto::DeviceStatus::id, status, proto::DeviceStatus::type);
                holder->setStatus(static_cast<DeviceStatus>(status));
            }
        });
    QObject::connect(
        holder,
        &DeviceStatusHolder::changed,
        modbusData,
        [modbusData](const auto& status)
        {
            modbusData->write(
                proto::DeviceStatus::id,
                static_cast<uint16_t>(status),
                proto::DeviceStatus::type);
        });
}

} // namespace mpk::drone::core
