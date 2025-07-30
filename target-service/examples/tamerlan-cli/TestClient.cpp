#include "TestClient.h"

#include "core/modbus/device/ChannelDataModbus.h"
#include "core/modbus/device/DeviceInfoModbus.h"
#include "core/modbus/device/GeoDataModbus.h"
#include "core/modbus/device/NetworkDataModbus.h"
#include "core/modbus/device/TransmissionDataModbus.h"
#include "core/proto/DeviceParams.h"
#include "core/proto/FileChunk.h"
#include "core/proto/FileTransferStatus.h"
#include "core/proto/JammerChannelParams.h"
#include "core/proto/NetworkFlags.h"
#include "core/proto/Protocol.h"

#ifdef OS_UNIX
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace mpk::drone
{

TestClient::TestClient(
    ModbusType type, std::string_view description, int timeout) :
  m_type(type)
{
    if (m_type == ModbusType::TCP)
    {
        auto tcpParams = parseAddress(description);
        m_ctx = modbus_new_tcp(tcpParams.host.data(), tcpParams.port);
        modbus_set_slave(m_ctx, tcpParams.serverAddress);
    }
    else
    {
        auto serialParams = parseSerial(description);
        m_ctx = modbus_new_rtu(
            serialParams.name.data(),
            serialParams.baudRate,
            serialParams.parity,
            serialParams.dataBits,
            serialParams.stopBits);
        modbus_set_slave(m_ctx, serialParams.serverAddress);
    }

    if (!m_ctx)
    {
        throw std::runtime_error(
            std::string("Failed to create modbus: ") + modbus_strerror(errno));
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    modbus_set_response_timeout(m_ctx, timeout / 1000, 1000 * (timeout % 1000));

    if (modbus_connect(m_ctx) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to connect to <") + std::string(description)
            + ">: " + modbus_strerror(errno));
    }
}

TestClient::~TestClient()
{
    modbus_close(m_ctx);
    modbus_free(m_ctx);
}

void TestClient::getDeviceData(bool sensor)
{
    uint16_t deviceStatus = 0;
    readInputRegisters(
        core::proto::DeviceStatus::address,
        core::proto::DeviceStatus::size,
        &deviceStatus);

    uint16_t modbusAddress = 0;
    readHoldingRegisters(
        core::proto::ModbusParams::address,
        core::proto::ModbusParams::size,
        &modbusAddress);

    auto networkData = core::NetworkDataModbus{};
    uint16_t port = 0;
    if (sensor)
    {
        auto networkDataRegs =
            std::vector<std::uint16_t>(sizeof(core::NetworkDataModbus) / 2);
        readHoldingRegisters(
            core::proto::NetworkData::address,
            core::proto::NetworkData::size,
            networkDataRegs.data());
        networkData = core::NetworkDataModbus::fromRegs(
            networkDataRegs.begin(), networkDataRegs.end());

        readHoldingRegisters(
            core::proto::ServicePort::address,
            core::proto::ServicePort::size,
            &port);
    }

    auto deviceInfoRegs =
        std::vector<std::uint16_t>(sizeof(core::DeviceInfoModbus) / 2);
    readInputRegisters(
        core::proto::DeviceInfo::address,
        core::proto::DeviceInfo::size,
        deviceInfoRegs.data());
    auto deviceInfo = core::DeviceInfoModbus::fromRegs(
        deviceInfoRegs.begin(), deviceInfoRegs.end());

    uint16_t channelsCount = 0;
    readInputRegisters(
        core::proto::ChannelsCount::address,
        core::proto::ChannelsCount::size,
        &channelsCount);

    auto geoDataRegs =
        std::vector<std::uint16_t>(sizeof(core::GeoDataModbus) / 2);
    readHoldingRegisters(
        core::proto::GeoData::address,
        core::proto::GeoData::size,
        geoDataRegs.data());
    auto geoData =
        core::GeoDataModbus::fromRegs(geoDataRegs.begin(), geoDataRegs.end());

    uint16_t protocolVersion = 0;
    readInputRegisters(
        core::proto::ProtocolVersion::address,
        core::proto::ProtocolVersion::size,
        &protocolVersion);

    auto deviceParams = core::DeviceParamsModbus{};
    float maxTemperature = 0.0F;
    if (!sensor)
    {
        auto deviceParamsRegs =
            std::vector<std::uint16_t>(sizeof(core::DeviceParamsModbus) / 2);
        readHoldingRegisters(
            core::proto::DeviceParams::address,
            core::proto::DeviceParams::size,
            deviceParamsRegs.data());
        deviceParams = core::DeviceParamsModbus::fromRegs(
            deviceParamsRegs.begin(), deviceParamsRegs.end());

        auto maxTemperatureRegs =
            std::vector<std::uint16_t>(core::proto::DeviceMaxTemperature::size);
        readInputRegisters(
            core::proto::DeviceMaxTemperature::address,
            core::proto::DeviceMaxTemperature::size,
            maxTemperatureRegs.data());
        auto it = maxTemperatureRegs.begin();
        std::tie(maxTemperature, it) = core::regsToFloat(it);
    }

    std::cout << "****** Device:" << std::endl;
    std::cout << "Status: " << deviceStatus << std::endl;
    std::cout << "Modbus address: " << modbusAddress << std::endl;
    if (sensor)
    {
        std::cout
            << "Network address: " << inet_ntoa({networkData.networkAddress})
            << std::endl;
        std::cout << "Netmask: " << inet_ntoa({networkData.netmask})
                  << std::endl;
        std::cout << "Gateway: " << inet_ntoa({networkData.gateway})
                  << std::endl;
        std::cout << "DNS: " << inet_ntoa({networkData.dns}) << std::endl;
        std::cout << "Network flags: " << networkData.flags << std::endl;
        std::cout << "Port: " << port << std::endl;
    }
    std::cout << "Model: "
              << std::string(deviceInfo.model.data(), deviceInfo.model.size())
              << std::endl;
    std::cout << "Serial: "
              << std::string(deviceInfo.serial.data(), deviceInfo.serial.size())
              << std::endl;
    std::cout << "Version: " << deviceInfo.version << std::endl;
    std::cout << "Revision: " << deviceInfo.revision << std::endl;
    std::cout
        << "Update date: " << (deviceInfo.updateDate & core::eightBitMask)
        << "."
        << ((deviceInfo.updateDate >> core::eightBit) & core::eightBitMask)
        << "."
        << ((deviceInfo.updateDate >> core::sixteenBit) & core::sixteenBitMask)
        << std::endl;
    std::cout << "Channels count: " << channelsCount << std::endl;
    std::cout << "Latitude: " << geoData.latitude << std::endl;
    std::cout << "Longitude: " << geoData.longitude << std::endl;
    std::cout << "Orientation: " << geoData.orientation << std::endl;
    std::cout << "Protocol version: " << protocolVersion << std::endl;
    if (!sensor)
    {
        std::cout << "Min voltage: " << deviceParams.minVoltage << std::endl;
        std::cout << "Max voltage: " << deviceParams.maxVoltage << std::endl;
        std::cout << "Max working temperature: " << deviceParams.maxTemperature
                  << std::endl;
        std::cout << "Channels delay: " << deviceParams.channelsDelay
                  << std::endl;
        std::cout << "Max time for current exceeding max value: "
                  << deviceParams.maxTimeForCurrent << std::endl;
        std::cout << "Max time for voltage below min value: "
                  << deviceParams.maxTimeForVoltage << std::endl;
        std::cout << "Max temperature: " << maxTemperature << std::endl;
    }
}

void TestClient::getSensorChannel(int channelIndex)
{
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextSensorChannelDataOffset;

    uint16_t status = 0;
    readInputRegisters(
        channelAddress + core::proto::ChannelStatus::relativeAddress,
        core::proto::ChannelStatus::size,
        &status);

    auto channelZoneRegs =
        std::vector<std::uint16_t>(sizeof(core::ChannelZoneModbus) / 2);
    readHoldingRegisters(
        channelAddress + core::proto::ChannelZone::relativeAddress,
        core::proto::ChannelZone::size,
        channelZoneRegs.data());
    auto channelZone = core::ChannelZoneModbus::fromRegs(
        channelZoneRegs.begin(), channelZoneRegs.end());

    auto channelParamsRegs =
        std::vector<std::uint16_t>(sizeof(core::ChannelParamsModbus) / 2);
    readInputRegisters(
        channelAddress + core::proto::ChannelParams::relativeAddress,
        core::proto::ChannelParams::size,
        channelParamsRegs.data());
    auto channelParams = core::ChannelParamsModbus::fromRegs(
        channelParamsRegs.begin(), channelParamsRegs.end());

    uint16_t objectsCount = 0;
    readInputRegisters(
        channelAddress + core::proto::ChannelObjectsCount::relativeAddress,
        core::proto::ChannelObjectsCount::size,
        &objectsCount);

    std::cout << "****** Channel " << (channelIndex + 1) << ":" << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "Direction: " << channelZone.direction << std::endl;
    std::cout << "Work angle: " << channelZone.workAngle << std::endl;
    std::cout << "Distance: " << channelZone.distance << std::endl;
    std::cout << "Sampling frequency: " << channelParams.samplingFrequency
              << std::endl;
    std::cout << "Bandwidth: " << channelParams.bandwidth << std::endl;
    std::cout << "Objects count: " << objectsCount << std::endl;

    auto regs =
        std::vector<std::uint16_t>(sizeof(core::TransmissionDataModbus) / 2);

    for (int i = 0; i < static_cast<int>(objectsCount); i++)
    {
        readInputRegisters(
            static_cast<int>(
                channelAddress + core::proto::Alarm::relativeAddress
                + i * regs.size()),
            core::proto::Alarm::size,
            regs.data());
        auto alarm =
            core::TransmissionDataModbus::fromRegs(regs.begin(), regs.end());
        std::cout << "****** Alarm #" << (i + 1) << ":" << std::endl;
        std::cout << "Number: " << alarm.number << std::endl;
        std::cout << "ID: " << alarm.id << std::endl;
        std::cout
            << "Name: " << std::string(alarm.name.data(), alarm.name.size())
            << std::endl;
        std::cout << "Begin freq: " << alarm.beginFreq << std::endl;
        std::cout << "End freq: " << alarm.endFreq << std::endl;
        std::cout << "Detection level: " << alarm.blocksCount << std::endl;
        std::cout << "Detection threshold: " << alarm.minQuantSquares
                  << std::endl;
        std::cout << "Detection probability: " << alarm.detectionProbability
                  << std::endl;
    }
}

void TestClient::getJammerChannel(int channelIndex)
{
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextJammerChannelDataOffset;

    uint16_t status = 0;
    readInputRegisters(
        channelAddress + core::proto::ChannelStatus::relativeAddress,
        core::proto::ChannelStatus::size,
        &status);

    uint8_t emissionStatus = 0;
    readCoils(
        channelAddress + core::proto::JammerChannelControl::relativeAddress,
        core::proto::JammerChannelControl::size,
        &emissionStatus);

    auto nameRegs =
        std::vector<std::uint16_t>(core::proto::JammerChannelName::size);
    std::array<char, core::proto::JammerChannelName::size> name = {};
    readInputRegisters(
        channelAddress + core::proto::JammerChannelName::relativeAddress,
        core::proto::JammerChannelName::size,
        nameRegs.data());
    core::regsToCharArray(name.data(), name.size(), nameRegs.begin());

    float voltage = 0.0F;
    auto voltageRegs =
        std::vector<std::uint16_t>(core::proto::JammerChannelVoltage::size);
    readInputRegisters(
        channelAddress + core::proto::JammerChannelVoltage::relativeAddress,
        core::proto::JammerChannelVoltage::size,
        voltageRegs.data());
    auto it = voltageRegs.begin();
    std::tie(voltage, it) = core::regsToFloat(it);

    float current = 0.0F;
    auto currentRegs =
        std::vector<std::uint16_t>(core::proto::JammerChannelCurrent::size);
    readInputRegisters(
        channelAddress + core::proto::JammerChannelCurrent::relativeAddress,
        core::proto::JammerChannelCurrent::size,
        currentRegs.data());
    it = currentRegs.begin();
    std::tie(current, it) = core::regsToFloat(it);

    auto paramsRegs =
        std::vector<std::uint16_t>(sizeof(core::JammerChannelParamsModbus) / 2);
    readHoldingRegisters(
        channelAddress + core::proto::JammerChannelParams::relativeAddress,
        core::proto::JammerChannelParams::size,
        paramsRegs.data());
    auto params = core::JammerChannelParamsModbus::fromRegs(
        paramsRegs.begin(), paramsRegs.end());

    std::cout << "****** Channel " << (channelIndex + 1) << ":" << std::endl;
    std::cout << "Status: " << status << std::endl;
    std::cout << "Emission status: " << static_cast<int>(emissionStatus)
              << std::endl;
    std::cout << "Name: " << std::string(name.data(), name.size()) << std::endl;
    std::cout << "Voltage: " << voltage << std::endl;
    std::cout << "Current: " << current << std::endl;
    std::cout << "Max current: " << params.maxCurrent << std::endl;
    std::cout << "Max impulse current: " << params.maxImpulseCurrent
              << std::endl;
}

void TestClient::getJammerData()
{
    auto regs = std::vector<std::uint16_t>(sizeof(core::JammerModbus) / 2);
    readHoldingRegisters(
        core::proto::JammerData::address,
        core::proto::JammerData::size,
        regs.data());
    auto jammer = core::JammerModbus::fromRegs(regs.begin(), regs.end());
    std::cout << "****** Jammer:" << std::endl;
    for (auto i = 0U; i < jammer.addresses.size(); i++)
    {
        std::cout << "Address " << i << " : " << jammer.addresses.at(i)
                  << std::endl;
    }
    std::cout << "Mode: " << jammer.mode << std::endl;
    std::cout << "Timeout: " << jammer.timeout << std::endl;
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void TestClient::sendFile(const std::string& fileName)
{
    std::ifstream input(fileName, std::ios_base::binary);
    if (!input.is_open())
    {
        std::cout << "Failed to open " << fileName << std::endl;
        return;
    }

    uint16_t status = 0;
    core::FileChunk<core::defaultChunkSize> chunk;

    auto setStatusAndWaitForChange =
        [this,
         &status,
         &chunk](core::FileTransferStatus statusToSet, bool writeChunk)
    {
        auto regs = std::vector<std::uint16_t>{};
        core::FileChunk<core::defaultChunkSize>::toRegs(
            chunk, std::back_inserter(regs));

        const int maxTryCount = 3;
        for (int i = 0; i < maxTryCount; ++i)
        {
            if (writeChunk)
            {
                writeHoldingRegisters(
                    core::proto::FileTransferChunk::address,
                    core::proto::FileTransferChunk::size,
                    regs.data());
            }

            using namespace std::chrono_literals;
            const std::vector<std::chrono::milliseconds> sleepIntervals = {
                20ms, 50ms, 100ms, 500ms};

            writeHoldingRegister(
                core::proto::FileTransferStatus::address,
                static_cast<uint16_t>(statusToSet));

            status = static_cast<uint16_t>(statusToSet);
            int intervalsElapsed = 0;
            const int maxIntervalsElapsed = 14;

            while (status == static_cast<uint16_t>(statusToSet)
                   && intervalsElapsed < maxIntervalsElapsed)
            {
                try
                {
                    auto sleepTime = sleepIntervals.at(std::min(
                        intervalsElapsed,
                        static_cast<int>(sleepIntervals.size()) - 1));
                    std::this_thread::sleep_for(sleepTime);
                    intervalsElapsed++;
                    readHoldingRegisters(
                        core::proto::FileTransferStatus::address,
                        core::proto::FileTransferStatus::size,
                        &status);
                }
                catch (std::exception& ex)
                {
                    std::cout << "Exception: " << ex.what() << std::endl;
                    status = static_cast<uint16_t>(statusToSet);
                }
            }
            if (status != static_cast<uint16_t>(statusToSet))
            {
                break;
            }
        }
        if (status == static_cast<uint16_t>(statusToSet))
        {
            writeHoldingRegister(
                core::proto::FileTransferStatus::address,
                static_cast<uint16_t>(core::FileTransferStatus::ERR));
        }
        return status != static_cast<uint16_t>(statusToSet);
    };

    readHoldingRegisters(
        core::proto::FileTransferStatus::address,
        core::proto::FileTransferStatus::size,
        &status);
    if (static_cast<core::FileTransferStatus>(status)
        != core::FileTransferStatus::CLR)
    {
        std::cout << "Incorrect status at start: "
                  << static_cast<core::FileTransferStatus>(status)
                  << " (Needed CLR)" << std::endl;
        input.close();
        return;
    }

    if (!setStatusAndWaitForChange(core::FileTransferStatus::RTS, false))
    {
        std::cout << "Error: timeout reached after RTS was set" << std::endl;
        input.close();
        return;
    }
    if (static_cast<core::FileTransferStatus>(status)
        != core::FileTransferStatus::RTR)
    {
        std::cout << "Incorrect status after RTS was set: "
                  << static_cast<core::FileTransferStatus>(status)
                  << " (Needed RTR)" << std::endl;
        input.close();
        return;
    }

    auto readBytes = [&chunk, &input]()
    {
        // NOLINTBEGIN (cppcoreguidelines-pro-type-reinterpret-cast)
        auto size = input.readsome(
            reinterpret_cast<char*>(chunk.bytes.data()),
            core::defaultChunkSize);
        // NOLINTEND (cppcoreguidelines-pro-type-reinterpret-cast)

        auto totalSize = size;
        while (totalSize < core::defaultChunkSize && size > 0)
        {
            // NOLINTBEGIN (cppcoreguidelines-pro-type-reinterpret-cast)
            size = input.readsome(
                reinterpret_cast<char*>(chunk.bytes.data() + totalSize),
                core::defaultChunkSize - totalSize);
            // NOLINTEND (cppcoreguidelines-pro-type-reinterpret-cast)
            totalSize += size;
        }
        return totalSize;
    };

    while ((chunk.size = readBytes()) > 0)
    {
        const int maxTryCount = 3;
        for (int i = 0; i < maxTryCount; ++i)
        {
            if (!setStatusAndWaitForChange(core::FileTransferStatus::CHW, true))
            {
                std::cout << "Error: timeout reached after CHW was set"
                          << std::endl;
                input.close();
                return;
            }
            if (static_cast<core::FileTransferStatus>(status)
                == core::FileTransferStatus::CHR)
            {
                break;
            }
        }
        if (static_cast<core::FileTransferStatus>(status)
            != core::FileTransferStatus::CHR)
        {
            std::cout << "Incorrect status after CHW was set: " << status
                      << " (Needed CHR)" << std::endl;
            input.close();
            return;
        }
    }
    input.close();

    writeHoldingRegister(
        core::proto::FileTransferStatus::address,
        static_cast<uint16_t>(core::FileTransferStatus::EOT));
}

void TestClient::setModbusAddress(uint16_t address)
{
    writeHoldingRegister(core::proto::ModbusParams::address, address);
    if (m_type == ModbusType::SERIAL)
    {
        modbus_set_slave(m_ctx, address);
    }
}

void TestClient::setNetworkAddress(const std::string& address)
{
    auto addrUint32 = inet_addr(address.data());
    std::vector<std::uint16_t> regs;
    core::uint32ToRegs(addrUint32, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, networkAddress) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setNetmask(const std::string& netmask)
{
    auto addrUint32 = inet_addr(netmask.data());
    std::vector<std::uint16_t> regs;
    core::uint32ToRegs(addrUint32, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, netmask) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setGateway(const std::string& gateway)
{
    auto addrUint32 = inet_addr(gateway.data());
    std::vector<std::uint16_t> regs;
    core::uint32ToRegs(addrUint32, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, gateway) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setDns(const std::string& dns)
{
    auto addrUint32 = inet_addr(dns.data());
    std::vector<std::uint16_t> regs;
    core::uint32ToRegs(addrUint32, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, dns) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setUseDhcp(bool value)
{
    uint16_t networkFlags = 0U;
    readHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, flags) / 2,
        1,
        &networkFlags);
    if (value)
    {
        networkFlags |= core::useDhcpFlag;
    }
    else
    {
        networkFlags &= (core::sixteenBitMask ^ core::useDhcpFlag);
    }
    writeHoldingRegister(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, flags) / 2,
        networkFlags);
}

void TestClient::setUseDnsFromDhcp(bool value)
{
    uint16_t networkFlags = 0U;
    readHoldingRegisters(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, flags) / 2,
        1,
        &networkFlags);
    if (value)
    {
        networkFlags |= core::useDnsFromDhcpFlag;
    }
    else
    {
        networkFlags &= (core::sixteenBitMask ^ core::useDnsFromDhcpFlag);
    }
    writeHoldingRegister(
        core::proto::NetworkData::address
            + offsetof(core::NetworkDataModbus, flags) / 2,
        networkFlags);
}

void TestClient::setNetworkPort(uint16_t value)
{
    writeHoldingRegister(core::proto::ServicePort::address, value);
}

void TestClient::setCoordinates(float latitude, float longitude)
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (latitude < -90.0F || latitude > 90.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect latitude: ") + std::to_string(latitude));
    }
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (longitude <= -180.0F || longitude > 180.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect longitude: ") + std::to_string(longitude));
    }

    std::vector<std::uint16_t> regs;
    core::floatToRegs(latitude, std::back_inserter(regs));
    core::floatToRegs(longitude, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::GeoData::address,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setOrientation(float orientation)
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (orientation < 0.0F || orientation >= 360.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect orientation: ")
            + std::to_string(orientation));
    }
    std::vector<std::uint16_t> regs;
    core::floatToRegs(orientation, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::GeoData::address
            + offsetof(core::GeoDataModbus, orientation) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setDirection(int channelIndex, float direction)
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (direction <= -180.0F || direction > 180.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect direction: ") + std::to_string(direction));
    }
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextSensorChannelDataOffset;
    std::vector<std::uint16_t> regs;
    core::floatToRegs(direction, std::back_inserter(regs));
    writeHoldingRegisters(
        static_cast<int>(
            channelAddress + core::proto::ChannelZone::relativeAddress
            + offsetof(core::ChannelZoneModbus, direction) / 2),
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setWorkAngle(int channelIndex, float workAngle)
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (workAngle < 0.0F || workAngle >= 360.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect work angle: ") + std::to_string(workAngle));
    }
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextSensorChannelDataOffset;
    std::vector<std::uint16_t> regs;
    core::floatToRegs(workAngle, std::back_inserter(regs));
    writeHoldingRegisters(
        static_cast<int>(
            channelAddress + core::proto::ChannelZone::relativeAddress
            + offsetof(core::ChannelZoneModbus, workAngle) / 2),
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setDistance(int channelIndex, float distance)
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    if (distance < 0.0F || distance > 100000.0F)
    {
        throw std::runtime_error(
            std::string("Incorrect distance: ") + std::to_string(distance));
    }
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextSensorChannelDataOffset;
    std::vector<std::uint16_t> regs;
    core::floatToRegs(distance, std::back_inserter(regs));
    writeHoldingRegisters(
        static_cast<int>(
            channelAddress + core::proto::ChannelZone::relativeAddress
            + offsetof(core::ChannelZoneModbus, distance) / 2),
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMinVoltage(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, minVoltage) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMaxVoltage(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, maxVoltage) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMaxTemperature(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, maxTemperature) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setChannelsDelay(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, channelsDelay) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMaxTimeCurrent(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, maxTimeForCurrent) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMaxTimeVoltage(float value)
{
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        core::proto::DeviceParams::address
            + offsetof(core::DeviceParamsModbus, maxTimeForVoltage) / 2,
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setEmissionAll(bool value)
{
    writeCoil(
        core::proto::JammerAllChannelsControl::address, value ? 0x01 : 0x00);
}

void TestClient::setSoftwareUpdate()
{
    writeCoil(core::proto::UpdateControl::address, 0x01);
}

void TestClient::setLicenseUpdate()
{
    writeCoil(core::proto::LicenseControl::address, 0x01);
}

void TestClient::setJammerAddresses(
    const std::array<uint16_t, mpk::drone::core::JammerModbus::jammersCount>&
        addressesArray)
{
    writeHoldingRegisters(
        core::proto::JammerData::address
            + offsetof(core::JammerModbus, addresses) / 2,
        static_cast<int>(addressesArray.size()),
        addressesArray.data());
}

void TestClient::setJammerMode(uint16_t value)
{
    writeHoldingRegister(
        core::proto::JammerData::address
            + offsetof(core::JammerModbus, mode) / 2,
        value);
}

void TestClient::setJammerTimeout(uint16_t value)
{
    writeHoldingRegister(
        core::proto::JammerData::address
            + offsetof(core::JammerModbus, timeout) / 2,
        value);
}

void TestClient::setJammerEmission(int channelIndex, bool value)
{
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextJammerChannelDataOffset;
    writeCoil(
        channelAddress + core::proto::JammerChannelControl::relativeAddress,
        value ? 0x01 : 0x00);
}

void TestClient::setMaxCurrent(int channelIndex, float value)
{
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextJammerChannelDataOffset;
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        static_cast<int>(
            channelAddress + core::proto::JammerChannelParams::relativeAddress
            + offsetof(core::JammerChannelParamsModbus, maxCurrent) / 2),
        static_cast<int>(regs.size()),
        regs.data());
}

void TestClient::setMaxImpulseCurrent(int channelIndex, float value)
{
    auto channelAddress =
        core::proto::firstChannelDataAddress
        + channelIndex * core::proto::nextJammerChannelDataOffset;
    std::vector<std::uint16_t> regs;
    core::floatToRegs(value, std::back_inserter(regs));
    writeHoldingRegisters(
        static_cast<int>(
            channelAddress + core::proto::JammerChannelParams::relativeAddress
            + offsetof(core::JammerChannelParamsModbus, maxImpulseCurrent) / 2),
        static_cast<int>(regs.size()),
        regs.data());
}

TestClient::TcpParams TestClient::parseAddress(std::string_view address)
{
    auto checkForDelimiterFound =
        [address](std::size_t pos, const std::string_view& str)
    {
        if (pos == str.npos)
        {
            throw std::runtime_error(
                std::string("Failed to parse address <") + std::string(address)
                + ">");
        }
    };

    TcpParams tcpParams;

    auto pos = address.find_first_of(':');
    checkForDelimiterFound(pos, address);
    tcpParams.host = std::string(address.substr(0, pos));

    auto substr = address.substr(pos + 1, address.size());
    pos = substr.find_first_of(':');
    checkForDelimiterFound(pos, substr);
    tcpParams.port = std::stoi(std::string(substr.substr(0, pos)));
    tcpParams.serverAddress =
        std::stoi(std::string(substr.substr(pos + 1, substr.size())));

    return tcpParams;
}

TestClient::SerialParams TestClient::parseSerial(std::string_view serialStr)
{
    auto checkForDelimiterFound =
        [serialStr](std::size_t pos, const std::string_view& str)
    {
        if (pos == str.npos)
        {
            throw std::runtime_error(
                std::string("Failed to parse serial <") + std::string(serialStr)
                + ">");
        }
    };

    SerialParams serialParams;

    auto pos = serialStr.find_first_of(':');
    checkForDelimiterFound(pos, serialStr);
    serialParams.name = std::string(serialStr.substr(0, pos));

    auto substr = serialStr.substr(pos + 1, serialStr.size());
    pos = substr.find_first_of(':');
    checkForDelimiterFound(pos, substr);
    serialParams.baudRate = std::stoi(std::string(substr.substr(0, pos)));

    substr = substr.substr(pos + 1, substr.size());
    pos = substr.find_first_of(':');
    checkForDelimiterFound(pos, substr);
    serialParams.parity = substr.substr(0, pos).at(0);

    substr = substr.substr(pos + 1, substr.size());
    pos = substr.find_first_of(':');
    checkForDelimiterFound(pos, substr);
    serialParams.dataBits = std::stoi(std::string(substr.substr(0, pos)));

    substr = substr.substr(pos + 1, substr.size());
    pos = substr.find_first_of(':');
    checkForDelimiterFound(pos, substr);
    serialParams.stopBits = std::stoi(std::string(substr.substr(0, pos)));
    serialParams.serverAddress =
        std::stoi(std::string(substr.substr(pos + 1, substr.size())));

    return serialParams;
}

void TestClient::readInputRegisters(
    int address, int numRegs, uint16_t* destination)
{
    if (modbus_read_input_registers(m_ctx, address, numRegs, destination) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to read input registers at <")
            + std::to_string(address) + ">: " + modbus_strerror(errno));
    }
}

void TestClient::readHoldingRegisters(
    int address, int numRegs, uint16_t* destination)
{
    if (modbus_read_registers(m_ctx, address, numRegs, destination) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to read holding registers at <")
            + std::to_string(address) + ">: " + modbus_strerror(errno));
    }
}

void TestClient::readCoils(int address, int numRegs, uint8_t* destination)
{
    if (modbus_read_bits(m_ctx, address, numRegs, destination) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to read coils at <") + std::to_string(address)
            + ">: " + modbus_strerror(errno));
    }
}

void TestClient::writeHoldingRegister(int address, uint16_t value)
{
    if (modbus_write_register(m_ctx, address, value) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to write holding register at <")
            + std::to_string(address) + ">: " + modbus_strerror(errno));
    }
}

void TestClient::writeHoldingRegisters(
    int address, int numRegs, const uint16_t* data)
{
    if (modbus_write_registers(m_ctx, address, numRegs, data) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to write holding registers at <")
            + std::to_string(address) + ">: " + modbus_strerror(errno));
    }
}

void TestClient::writeCoil(int address, int status)
{
    if (modbus_write_bit(m_ctx, address, status) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to write coil at <") + std::to_string(address)
            + ">: " + modbus_strerror(errno));
    }
}

void TestClient::writeCoils(int address, int numRegs, const uint8_t* data)
{
    if (modbus_write_bits(m_ctx, address, numRegs, data) < 0)
    {
        throw std::runtime_error(
            std::string("Failed to write coils at <") + std::to_string(address)
            + ">: " + modbus_strerror(errno));
    }
}

} // namespace mpk::drone
