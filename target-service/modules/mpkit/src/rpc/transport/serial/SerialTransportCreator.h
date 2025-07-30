#pragma once

#include <memory>

class QJsonObject;

namespace rpc
{

class Transport;

/**
 * The parameters should have the following structure
{
    "name": serial port device name
    "baudRate": BaudRate enum value,
    "dataBits": DataBits enum value,
    "parity": Parity enum value
    "stopBits": StopBits enum value
}
*/
std::unique_ptr<Transport> createSerialTransport(const QJsonObject& parameters);

} // namespace rpc
