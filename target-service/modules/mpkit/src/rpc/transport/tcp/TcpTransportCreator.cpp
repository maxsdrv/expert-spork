#include "TcpTransportCreator.h"

#include "rpc/transport/TransportExceptions.h"

#include "TcpClient.h"
#include "TcpMultiServer.h"
#include "TcpSingleServer.h"

namespace rpc
{

std::unique_ptr<Transport> createTcpTransport(const QJsonObject& parameters)
{
    constexpr auto modeTag = "mode";
    constexpr auto hostTag = "host";
    constexpr auto portTag = "port";

    auto mode = json::fromObject<QString>(parameters, modeTag);
    auto host = json::fromObject<QHostAddress>(parameters, hostTag);
    auto port = json::fromObject<quint16>(parameters, portTag);

    SocketAddress socketAddress{host, port};

    if (mode == "client")
    {
        return std::make_unique<TcpClient>(std::move(socketAddress));
    }
    if (mode == "single_server")
    {
        return std::make_unique<TcpSingleServer>(socketAddress);
    }
    if (mode == "multi_server")
    {
        return std::make_unique<TcpMultiServer>(socketAddress);
    }

    BOOST_THROW_EXCEPTION(exception::TransportCreationFailed("Invalid tcp transport type"));
}

} // namespace rpc
