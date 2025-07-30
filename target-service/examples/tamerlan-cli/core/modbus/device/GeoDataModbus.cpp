#include "GeoDataModbus.h"

#include "core/modbus/FloatModbus.h"
#include "core/modbus/ModbusData.h"
#include "core/proto/Protocol.h"

namespace mpk::drone::core
{

GeoData convert(const GeoDataModbus& data)
{
    return {{data.latitude, data.longitude}, data.orientation};
}

GeoDataModbus convert(const GeoData& data)
{
    return {
        static_cast<float>(data.position.latitude),
        static_cast<float>(data.position.longitude),
        static_cast<float>(data.azimuth)};
}

void connectGeoDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData, gsl::not_null<GeoDataHolder*> holder)
{
    modbusData->emplace<GeoDataModbus>(
        proto::GeoData::id,
        proto::GeoData::address,
        proto::GeoData::type,
        convert(holder->data()));
    QObject::connect(
        modbusData,
        &ModbusData::modified,
        holder,
        [modbusData, holder](const auto& id)
        {
            if (id == proto::GeoData::id)
            {
                GeoDataModbus data;
                modbusData->read(proto::GeoData::id, data, proto::GeoData::type);
                holder->setData(convert(data));
            }
        });
    QObject::connect(
        holder,
        &GeoDataHolder::changed,
        modbusData,
        [modbusData](const auto& data)
        {
            modbusData->write(
                proto::GeoData::id, convert(data), proto::GeoData::type);
        });
}

} // namespace mpk::drone::core
