#pragma once

#include "rpc/transport/Transport.h"

#include <memory>

namespace rpc
{

struct SocketAddress;
struct SslData;
class TcpMessenger;
class SslServer;

class SslSingleServer : public Transport
{

public:
    SslSingleServer(const SocketAddress& address, SslData sslData);
    ~SslSingleServer() override;

    void send(const QByteArray& array) override;

private:
    std::unique_ptr<SslServer> m_server;
    std::shared_ptr<TcpMessenger> m_messenger;
};

} // namespace rpc
