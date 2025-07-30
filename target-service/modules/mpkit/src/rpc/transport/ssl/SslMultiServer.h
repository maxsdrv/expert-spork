#pragma once

#include "rpc/transport/Transport.h"

#include <map>
#include <memory>

namespace rpc
{

struct SocketAddress;
struct SslData;
class TcpMessenger;
class SslServer;

class SslMultiServer : public Transport
{

public:
    SslMultiServer(const SocketAddress& address, SslData sslData);
    ~SslMultiServer() override;

    void send(const QByteArray& array) override;

private:
    std::unique_ptr<SslServer> m_server;
    std::map<SocketAddress, std::shared_ptr<TcpMessenger>> m_messengers;
};

} // namespace rpc
