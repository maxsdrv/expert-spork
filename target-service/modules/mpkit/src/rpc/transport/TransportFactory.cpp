#include "rpc/transport/TransportFactory.h"

#include "rpc/transport/TransportExceptions.h"
#include "rpc/transport/serial/SerialTransportCreator.h"
#include "rpc/transport/ssl/SslTransportCreator.h"
#include "rpc/transport/tcp/TcpTransportCreator.h"
#include "rpc/transport/udp/UdpTransportCreator.h"

#include "json/parsers/QString.h"

namespace rpc
{

namespace
{

constexpr auto typeTag = "type";

} // namespace

TransportFactory::TransportFactory()
{
    m_creators.insert({"udp", createUdpTransport});
    m_creators.insert({"tcp", createTcpTransport});
    m_creators.insert({"ssl", createSslTransport});
    m_creators.insert({"serial", createSerialTransport});
}

std::unique_ptr<Transport> TransportFactory::create(const QJsonObject& description)
{
    auto type = json::fromObject<QString>(description, typeTag);
    auto it = m_creators.find(type);
    if (it == m_creators.end())
    {
        BOOST_THROW_EXCEPTION(exception::TransportCreationFailed(
            std::string("Unsupported transport type [") + type.toStdString() + "]"));
    }

    return it->second(description);
}

} // namespace rpc
