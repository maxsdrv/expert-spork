#pragma once

#include <memory>

class QJsonObject;

namespace rpc
{

class Transport;

/**
 * The parameters should have the following structure
{
    "listen": {
        "host": "host",
        "port": portNumber
    },
    "destination": {
        "host": "host",
        "port": portNumber
    }
}
where "listen" is listen endpoint parameters
    "destination" is destination endpoint parameters

    "host" is one of "any", "localhost" or any valid ip4 address.
    "port" is udp port
*/
std::unique_ptr<Transport> createUdpTransport(const QJsonObject& parameters);

} // namespace rpc
