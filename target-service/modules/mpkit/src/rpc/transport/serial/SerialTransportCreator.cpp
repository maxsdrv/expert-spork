#include "SerialTransportCreator.h"

#include "rpc/transport/serial/SerialEndpoint.h"

#include "json/ObjectParser.h"
#include "json/parsers/General.h"

namespace rpc
{

std::unique_ptr<Transport> createSerialTransport(const QJsonObject& parameters)
{
    constexpr auto nameTag = "name";
    constexpr auto baudRateTag = "baudRate";
    constexpr auto dataBitsTag = "dataBits";
    constexpr auto parityTag = "parity";
    constexpr auto stopBitsTag = "stopBits";

    auto const parser = json::ObjectParser(parameters);

    auto const name = parser.get<QString>(nameTag);
    auto const baudRate = parser.get<QSerialPort::BaudRate>(baudRateTag);
    auto const dataBits = parser.get<QSerialPort::DataBits>(dataBitsTag);
    auto const parity = parser.get<QSerialPort::Parity>(parityTag);
    auto const stopBits = parser.get<QSerialPort::StopBits>(stopBitsTag);

    return std::make_unique<SerialEndpoint>(name, baudRate, dataBits, parity, stopBits);
}

} // namespace rpc
