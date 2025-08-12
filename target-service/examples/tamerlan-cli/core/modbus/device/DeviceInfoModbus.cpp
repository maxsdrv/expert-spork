#include "DeviceInfoModbus.h"

#include "core/modbus/ModbusData.h"
#include "core/proto/Protocol.h"

namespace mpk::drone::core
{

DeviceInfoModbus convert(const DeviceInfo& data)
{
    std::array<char, DeviceInfoModbus::modelMaxSize> model = {0};
    std::copy_n(
        data.model.begin(),
        std::min(
            DeviceInfoModbus::modelMaxSize,
            static_cast<int>(data.model.size())),
        model.begin());
    std::array<char, DeviceInfoModbus::serialMaxSize> serial = {0};
    std::copy_n(
        data.serial.begin(),
        std::min(
            DeviceInfoModbus::serialMaxSize,
            static_cast<int>(data.serial.size())),
        serial.begin());

    uint32_t version = 0U;
    version |= static_cast<uint32_t>(data.version.build) & eightBitMask;
    version |=
        (static_cast<uint32_t>(data.version.number) & eightBitMask) << eightBit;
    version |= (static_cast<uint32_t>(data.version.minor) & eightBitMask)
               << 2 * eightBit;
    version |= (static_cast<uint32_t>(data.version.major) & eightBitMask)
               << 3 * eightBit;
    const int hexBase = 16;
    auto revision = data.revision.toULong(nullptr, hexBase);
    uint32_t updateDate = 0U;
    updateDate |= static_cast<uint32_t>(data.lastUpdated.day()) & eightBitMask;
    updateDate |=
        (static_cast<uint32_t>(data.lastUpdated.month()) & eightBitMask)
        << eightBit;
    updateDate |=
        (static_cast<uint32_t>(data.lastUpdated.year()) & eightBitMask)
        << 2 * eightBit;
    return {model, serial, version, revision, updateDate};
}

void connectDeviceInfoWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<DeviceInfoHolder*> holder)
{
    modbusData->emplace<DeviceInfoModbus>(
        proto::DeviceInfo::id,
        proto::DeviceInfo::address,
        proto::DeviceInfo::type,
        convert(holder->info()));
}

} // namespace mpk::drone::core
