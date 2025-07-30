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
    "port": port number,
    "private_key": private key file name,
    "certificate": local certificate file name,
    "ca_certificate": CA certificate file name
}
*/
std::unique_ptr<Transport> createSslTransport(const QJsonObject& parameters);

} // namespace rpc
