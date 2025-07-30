#pragma once

#include "core/proto/JammerData.h"

#include <modbus/modbus.h>

#include <array>
#include <string>
#include <string_view>

namespace mpk::drone
{

class TestClient
{
public:
    enum ModbusType
    {
        TCP,
        SERIAL
    };

public:
    explicit TestClient(
        ModbusType type, std::string_view description, int timeout);
    ~TestClient();
    TestClient(const TestClient& other) = delete;

    void getDeviceData(bool sensor);
    void getSensorChannel(int channelIndex);
    void getJammerChannel(int channelIndex);
    void getJammerData();
    void sendFile(const std::string& fileName);

    void setModbusAddress(uint16_t address);
    void setNetworkAddress(const std::string& address);
    void setNetmask(const std::string& netmask);
    void setGateway(const std::string& gateway);
    void setDns(const std::string& dns);
    void setUseDhcp(bool value);
    void setUseDnsFromDhcp(bool value);
    void setNetworkPort(uint16_t value);
    void setCoordinates(float latitude, float longitude);
    void setOrientation(float orientation);
    void setDirection(int channelIndex, float direction);
    void setWorkAngle(int channelIndex, float workAngle);
    void setDistance(int channelIndex, float distance);

    void setMinVoltage(float value);
    void setMaxVoltage(float value);
    void setMaxTemperature(float value);
    void setChannelsDelay(float value);
    void setMaxTimeCurrent(float value);
    void setMaxTimeVoltage(float value);
    void setEmissionAll(bool value);

    void setSoftwareUpdate();
    void setLicenseUpdate();

    void setJammerAddresses(
        const std::array<uint16_t, mpk::drone::core::JammerModbus::jammersCount>&
            addressesArray);
    void setJammerMode(uint16_t value);
    void setJammerTimeout(uint16_t value);

    void setJammerEmission(int channelIndex, bool value);
    void setMaxCurrent(int channelIndex, float value);
    void setMaxImpulseCurrent(int channelIndex, float value);

private:
    struct TcpParams
    {
        std::string host;
        int port;
        int serverAddress;
    };

    struct SerialParams
    {
        std::string name;
        int baudRate;
        char parity;
        int dataBits;
        int stopBits;
        int serverAddress;
    };

    static TcpParams parseAddress(std::string_view address);
    static SerialParams parseSerial(std::string_view serialStr);

    void readInputRegisters(int address, int numRegs, uint16_t* destination);
    void readHoldingRegisters(int address, int numRegs, uint16_t* destination);
    void readCoils(int address, int numRegs, uint8_t* destination);

    void writeHoldingRegister(int address, uint16_t value);
    void writeHoldingRegisters(int address, int numRegs, const uint16_t* data);
    void writeCoil(int address, int status);
    void writeCoils(int address, int numRegs, const uint8_t* data);

private:
    ModbusType m_type;
    modbus_t* m_ctx = nullptr;
};

} // namespace mpk::drone
