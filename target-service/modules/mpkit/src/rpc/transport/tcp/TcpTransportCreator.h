#pragma once

#include <memory>

class QJsonObject;

namespace rpc
{

class Transport;

/**
 * The parameters should have the following structure
{
    "mode": "client"/"single_server"/"multi_server"
    "host": "any"/"localhost"/hostname,
    "port": port number
}
*/
std::unique_ptr<Transport> createTcpTransport(const QJsonObject& parameters);

} // namespace rpc
