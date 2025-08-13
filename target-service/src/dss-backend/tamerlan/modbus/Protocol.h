#pragma once

namespace mpk::dss::backend::tamerlan::proto
{

struct DeviceStatus
{
    static constexpr auto address = 0x0000;
    static constexpr auto id = "device_status";
    static constexpr auto size = 1;
};

struct ModbusParams
{
    static constexpr auto address = 0x0001;
    static constexpr auto id = "modbus_params";
    static constexpr auto size = 1;
};

struct NetworkData
{
    static constexpr auto address = 0x0002;
    static constexpr auto id = "network_data";
    static constexpr auto size = 9;
};

struct ServicePort
{
    static constexpr auto address = 0x000B;
    static constexpr auto id = "service_port";
    static constexpr auto size = 1;
};

struct ChannelsCount
{
    static constexpr auto address = 0x0012;
    static constexpr auto id = "channels_count";
    static constexpr auto size = 1;
};

struct GeoData
{
    static constexpr auto address = 0x0013;
    static constexpr auto id = "geo_data";
    static constexpr auto size = 6;
};

struct ProtocolVersion
{
    static constexpr auto address = 0x0019;
    static constexpr auto id = "protocol_version";
    static constexpr auto size = 1;
};

struct DeviceInfo
{
    static constexpr auto address = 0x0100;
    static constexpr auto id = "device_info";
    static constexpr auto size = 48;
};

struct DeviceParams
{
    static constexpr auto address = 0x0F00;
    static constexpr auto id = "device_params";
    static constexpr auto size = 12;
};

struct JammerAllChannelsControl
{
    static constexpr auto address = 0x0020;
    static constexpr auto id = "jammer_all_channels_control";
    static constexpr auto size = 1;
};

struct DeviceMaxTemperature
{
    static constexpr auto address = 0x0021;
    static constexpr auto id = "device_max_temperature";
    static constexpr auto size = 2;
};

constexpr auto firstChannelDataAddress = 0x1000;
constexpr auto nextSensorChannelDataOffset = 0x0880;
constexpr auto nextJammerChannelDataOffset = 0x0100;
constexpr auto channelDataIdPrefix = "channel";
constexpr auto maxSensorChannelsCount = 4;
constexpr auto maxSensorChannelObjectsCount = 32;
constexpr auto maxJammerChannelsCount = 16;

struct ChannelStatus
{
    static constexpr auto relativeAddress = 0x0000;
    static constexpr auto suffix = "status";
    static constexpr auto size = 1;
};

struct ChannelZone
{
    static constexpr auto relativeAddress = 0x0001;
    static constexpr auto suffix = "zone";
    static constexpr auto size = 6;
};

struct ChannelParams
{
    static constexpr auto relativeAddress = 0x0007;
    static constexpr auto suffix = "params";
    static constexpr auto size = 4;
};

struct ChannelObjectsCount
{
    static constexpr auto relativeAddress = 0x000B;
    static constexpr auto suffix = "objects_count";
    static constexpr auto size = 1;
};

struct Alarm
{
    static constexpr auto relativeAddress = 0x0080;
    static constexpr auto suffix = "alarm";
    static constexpr auto size = 64;
};

struct JammerChannelControl
{
    static constexpr auto relativeAddress = 0x0001;
    static constexpr auto suffix = "jammer_control";
    static constexpr auto size = 1;
};

struct JammerChannelName
{
    static constexpr auto relativeAddress = 0x0002;
    static constexpr auto suffix = "jammer_name";
    static constexpr auto size = 32;
};

struct JammerChannelVoltage
{
    static constexpr auto relativeAddress = 0x0023;
    static constexpr auto suffix = "jammer_voltage";
    static constexpr auto size = 2;
};

struct JammerChannelCurrent
{
    static constexpr auto relativeAddress = 0x0025;
    static constexpr auto suffix = "jammer_current";
    static constexpr auto size = 2;
};

struct JammerChannelParams
{
    static constexpr auto relativeAddress = 0x0080;
    static constexpr auto suffix = "jammer_params";
    static constexpr auto size = 4;
};

struct FileTransferStatus
{
    static constexpr auto address = 0x3200;
    static constexpr auto id = "transfer_status";
    static constexpr auto size = 1;
};

struct FileTransferChunk
{
    static constexpr auto address = 0x3201;
    static constexpr auto id = "transfer_chunk";
    static constexpr auto size = 121;
};

struct UpdateControl
{
    static constexpr auto address = 0x3300;
    static constexpr auto id = "update_control";
    static constexpr auto size = 1;
};

struct LicenseControl
{
    static constexpr auto address = 0x3301;
    static constexpr auto id = "license_control";
    static constexpr auto size = 1;
};

struct JammerData
{
    static constexpr auto address = 0x3400;
    static constexpr auto id = "jammer_data";
    static constexpr auto size = 18;
};

} // namespace mpk::dss::backend::tamerlan::proto
