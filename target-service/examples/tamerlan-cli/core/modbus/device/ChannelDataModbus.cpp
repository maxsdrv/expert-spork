#include "ChannelDataModbus.h"

#include "core/modbus/FloatModbus.h"
#include "core/modbus/ModbusData.h"
#include "core/modbus/device/TransmissionDataModbus.h"
#include "core/proto/Protocol.h"

#include <QRegExp>

namespace mpk::drone::core
{

ChannelZone convert(const ChannelZoneModbus& zone)
{
    return {zone.direction, zone.workAngle, zone.distance};
}

ChannelZoneModbus convert(const ChannelZone& zone)
{
    return {
        static_cast<float>(zone.azimuth),
        static_cast<float>(zone.span),
        static_cast<float>(zone.distance)};
}

ChannelParams convert(const ChannelParamsModbus& params)
{
    return {params.samplingFrequency, params.bandwidth};
}

ChannelParamsModbus convert(const ChannelParams& params)
{
    return {params.sampleRate, params.bandwidth};
}

TransmissionDataModbus convert(const TransmissionData& data)
{
    std::array<char, TransmissionDataModbus::nameMaxSize> name = {0};
    std::copy_n(
        data.name.begin(),
        std::min(
            TransmissionDataModbus::nameMaxSize,
            static_cast<int>(data.name.size())),
        name.begin());
    return {
        .id = static_cast<uint16_t>(data.id),
        .name = name,
        .blocksCount = data.blocksCount,
        .minQuantSquares = data.minBlocksCount,
        .beginFreq = data.frequencyRange.begin(),
        .endFreq = data.frequencyRange.end(),
        .detectionProbability = static_cast<float>(data.detectionProbability)};
}

void connectChannelDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<ChannelDataHolder*> holder)
{
    for (int channelIndex = 0; channelIndex < holder->count(); channelIndex++)
    {
        auto prefix = proto::channelDataIdPrefix + std::to_string(channelIndex);
        auto startAddress = proto::firstChannelDataAddress
                            + channelIndex * proto::nextSensorChannelDataOffset;
        modbusData->emplace<uint16_t>(
            prefix + proto::ChannelStatus::suffix,
            startAddress + proto::ChannelStatus::relativeAddress,
            proto::ChannelStatus::type,
            static_cast<uint16_t>(holder->status(channelIndex)));
        modbusData->emplace<ChannelZoneModbus>(
            prefix + proto::ChannelZone::suffix,
            startAddress + proto::ChannelZone::relativeAddress,
            proto::ChannelZone::type,
            convert(holder->zone(channelIndex)));
        modbusData->emplace<ChannelParamsModbus>(
            prefix + proto::ChannelParams::suffix,
            startAddress + proto::ChannelParams::relativeAddress,
            proto::ChannelParams::type,
            convert(holder->params(channelIndex)));

        auto transmissions = holder->transmissions(channelIndex);
        modbusData->emplace<uint16_t>(
            prefix + proto::ChannelObjectsCount::suffix,
            startAddress + proto::ChannelObjectsCount::relativeAddress,
            proto::ChannelObjectsCount::type,
            static_cast<uint16_t>(transmissions.size()));
        const int maxTransmissionsCount = 8;
        for (int alarmIndex = 0; alarmIndex < maxTransmissionsCount;
             alarmIndex++)
        {
            modbusData->emplace<TransmissionDataModbus>(
                prefix + proto::Alarm::suffix + std::to_string(alarmIndex),
                startAddress + proto::Alarm::relativeAddress
                    + alarmIndex
                          * static_cast<int>(
                              sizeof(TransmissionDataModbus) / 2),
                proto::Alarm::type,
                convert(TransmissionData{}));
        }
    }
    QObject::connect(
        modbusData,
        &ModbusData::modified,
        holder,
        [modbusData, holder](const auto& id)
        {
            QRegExp channelRegExp(
                QString("%1(\\d+)(\\w+)").arg(proto::channelDataIdPrefix));
            if (channelRegExp.exactMatch(QString::fromStdString(id)))
            {
                auto channelIndex = channelRegExp.cap(1).toInt();
                auto idSuffix = channelRegExp.cap(2);
                if (idSuffix == proto::ChannelZone::suffix)
                {
                    ChannelZoneModbus zone;
                    modbusData->read(id, zone, proto::ChannelZone::type);
                    holder->setZone(channelIndex, convert(zone));
                }
            }
        });
    QObject::connect(
        holder,
        &ChannelDataHolder::statusChanged,
        modbusData,
        [modbusData](int index, const auto& status)
        {
            modbusData->write(
                proto::channelDataIdPrefix + std::to_string(index)
                    + proto::ChannelStatus::suffix,
                static_cast<uint16_t>(status),
                proto::ChannelStatus::type);
        });
    QObject::connect(
        holder,
        &ChannelDataHolder::zoneChanged,
        modbusData,
        [modbusData](int index, const auto& zone)
        {
            modbusData->write(
                proto::channelDataIdPrefix + std::to_string(index)
                    + proto::ChannelZone::suffix,
                convert(zone),
                proto::ChannelZone::type);
        });
    QObject::connect(
        holder,
        &ChannelDataHolder::paramsChanged,
        modbusData,
        [modbusData](int index, const auto& params)
        {
            modbusData->write(
                proto::channelDataIdPrefix + std::to_string(index)
                    + proto::ChannelParams::suffix,
                convert(params),
                proto::ChannelParams::type);
        });
    QObject::connect(
        holder,
        &ChannelDataHolder::transmissionsChanged,
        modbusData,
        [modbusData](int index, const auto& transmissions)
        {
            modbusData->write(
                proto::channelDataIdPrefix + std::to_string(index)
                    + proto::ChannelObjectsCount::suffix,
                static_cast<uint16_t>(transmissions.size()),
                proto::ChannelObjectsCount::type);
            for (int alarmIndex = 0;
                 alarmIndex < static_cast<int>(transmissions.size());
                 alarmIndex++)
            {
                modbusData->write(
                    proto::channelDataIdPrefix + std::to_string(index)
                        + proto::Alarm::suffix + std::to_string(alarmIndex),
                    convert(transmissions.at(alarmIndex)),
                    proto::Alarm::type);
            }
        });
}

} // namespace mpk::drone::core
