#include "UdpTransportCreator.h"

#include "rpc/transport/Socket.h"

#include "UdpEndpoint.h"

namespace rpc
{

std::unique_ptr<Transport> createUdpTransport(const QJsonObject& parameters)
{
    constexpr auto listenTag = "listen";
    constexpr auto destinationTag = "destination";

    auto destination = parameters.contains(destinationTag) ?
                           json::fromObject<SocketAddress>(parameters, destinationTag) :
                           SocketAddress{QHostAddress::Any, 0};

    auto listen = json::fromObject<SocketAddress>(parameters, listenTag);
    return std::make_unique<UdpEndpoint>(destination, listen);
}

} // namespace rpc
