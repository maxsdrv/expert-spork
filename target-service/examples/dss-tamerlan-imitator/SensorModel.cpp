#include "SensorModel.h"

#include "SerialNumberGenerator.h"
#include "modbus/device/ChannelDataModbus.h"
#include "modbus/device/DeviceInfoModbus.h"

#include "dss-backend/tamerlan/modbus/Protocol.h"
#include "dss-common/modbus/byteorderutils.h"

#include <QDataStream>

#include <optional>

namespace mpk::dss::imitator::tamerlan::sensor
{
using regValueType = quint16;

ModbusDataUnits Model::createModbusDataUnits(
    const std::optional<SensorChannelsParameters>& channelsParameters)
{
    using namespace mpk::dss::backend::tamerlan;

    ModbusDataUnits registers;

    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::DeviceStatus::address,
        proto::DeviceStatus::size);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::ModbusParams::address,
        proto::ModbusParams::size);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::NetworkData::address,
        proto::NetworkData::size);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::ServicePort::address,
        proto::ServicePort::size);

    auto numChannels = (channelsParameters) ? channelsParameters->size() :
                                              proto::maxSensorChannelsCount;
    auto numChannelsVal = toByteOrderValue(
        static_cast<regValueType>(numChannels), ByteOrder::LittleEndian);
    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::ChannelsCount::address,
        proto::ChannelsCount::size);
    registers.back().setValue(0, numChannelsVal);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::GeoData::address,
        proto::GeoData::size);
    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::ProtocolVersion::address,
        proto::ProtocolVersion::size);
    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::DeviceInfo::address,
        proto::DeviceInfo::size);
    auto& deviceInfoUnit = registers.back();
    SerialNumberGenerator serialNumberGenerator;
    DeviceInfoModbus deviceInfoModbus{
        .model = "Tamerlan Sensor Imitator",
        .serial =
            serialNumberGenerator.generate(DeviceInfoModbus::serialMaxSize)};
    deviceInfoModbus.store(deviceInfoUnit);
    if (channelsParameters)
    {
        int channelNum{0};
        for (const auto& param: channelsParameters.value())
        {
            auto channelOffset =
                proto::firstChannelDataAddress
                + proto::nextSensorChannelDataOffset * channelNum++;

            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::ChannelStatus::relativeAddress,
                proto::ChannelStatus::size);

            auto statusValue = toByteOrderValue(
                static_cast<regValueType>(param.status),
                ByteOrder::LittleEndian);
            registers.back().setValue(0, statusValue);

            auto channelZoneUnit = QModbusDataUnit{
                QModbusDataUnit::HoldingRegisters,
                static_cast<int>(
                    channelOffset + proto::ChannelZone::relativeAddress),
                proto::ChannelZone::size};
            param.channelZone.store(channelZoneUnit);
            registers.emplace_back(channelZoneUnit);

            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::ChannelParams::relativeAddress,
                proto::ChannelParams::size);

            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::ChannelObjectsCount::relativeAddress,
                proto::ChannelObjectsCount::size);
            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::Alarm::relativeAddress,
                proto::Alarm::size);
        }
    }
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::FileTransferStatus::address,
        proto::FileTransferStatus::size);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::FileTransferChunk::address,
        proto::FileTransferChunk::size);
    registers.emplace_back(
        QModbusDataUnit::Coils,
        proto::UpdateControl::address,
        proto::UpdateControl::size);
    registers.emplace_back(
        QModbusDataUnit::Coils,
        proto::LicenseControl::address,
        proto::LicenseControl::size);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::JammerData::address,
        proto::JammerData::size);

    return registers;
}

} // namespace mpk::dss::imitator::tamerlan::sensor
