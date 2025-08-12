#include "JammerModel.h"

#include "SerialNumberGenerator.h"
#include "converters/StringVsModbusDataUnit.h"
#include "modbus/device/DeviceInfoModbus.h"

#include "dss-backend/tamerlan/modbus/Protocol.h"
#include "dss-common/modbus/byteorderutils.h"

#include <QDataStream>
#include <QModbusDataUnit>

#include <algorithm>
#include <optional>

namespace mpk::dss::imitator::tamerlan::jammer
{

using regValueType = quint16;

ModbusDataUnits Model::createModbusDataUnits(
    const std::optional<JammerChannelsParameters>& channelsParameters)
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
        QModbusDataUnit::InputRegisters,
        proto::ChannelsCount::address,
        proto::ChannelsCount::size);
    auto numChannels = (channelsParameters) ? channelsParameters->size() :
                                              proto::maxJammerChannelsCount;
    auto numChannelsVal = toByteOrderValue(
        static_cast<regValueType>(numChannels), ByteOrder::LittleEndian);
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
        QModbusDataUnit::Coils,
        proto::JammerAllChannelsControl::address,
        proto::JammerAllChannelsControl::size);
    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::DeviceMaxTemperature::address,
        proto::DeviceMaxTemperature::size);
    registers.emplace_back(
        QModbusDataUnit::InputRegisters,
        proto::DeviceInfo::address,
        proto::DeviceInfo::size);
    auto& deviceInfoUnit = registers.back();
    SerialNumberGenerator serialNumberGenerator;
    DeviceInfoModbus deviceInfoModbus{
        .model = "Tamerlan Jammer Imitator",
        .serial =
            serialNumberGenerator.generate(DeviceInfoModbus::serialMaxSize)};
    deviceInfoModbus.store(deviceInfoUnit);
    registers.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        proto::DeviceParams::address,
        proto::DeviceParams::size);
    if (channelsParameters)
    {
        int channelNum{0};
        for (const auto& param: channelsParameters.value())
        {
            auto channelOffset =
                proto::firstChannelDataAddress
                + proto::nextJammerChannelDataOffset * channelNum++;

            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::ChannelStatus::relativeAddress,
                proto::ChannelStatus::size);
            registers.emplace_back(
                QModbusDataUnit::Coils,
                channelOffset + proto::JammerChannelControl::relativeAddress,
                proto::JammerChannelControl::size);
            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::JammerChannelName::relativeAddress,
                proto::JammerChannelName::size);

            StringVsModbusDataUnit::store(
                param.name, proto::JammerChannelName::size, registers.back());

            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::JammerChannelVoltage::relativeAddress,
                proto::JammerChannelVoltage::size);
            registers.emplace_back(
                QModbusDataUnit::InputRegisters,
                channelOffset + proto::JammerChannelCurrent::relativeAddress,
                proto::JammerChannelCurrent::size);
            registers.emplace_back(
                QModbusDataUnit::HoldingRegisters,
                channelOffset + proto::JammerChannelParams::relativeAddress,
                proto::JammerChannelParams::size);
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

    return registers;
}

} // namespace mpk::dss::imitator::tamerlan::jammer
