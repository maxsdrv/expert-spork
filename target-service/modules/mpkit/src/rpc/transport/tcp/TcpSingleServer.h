#pragma once

#include "rpc/transport/Transport.h"

#include <memory>

class QTcpServer;

namespace rpc
{

struct SocketAddress;
class TcpMessenger;

class TcpSingleServer : public Transport
{

public:
    explicit TcpSingleServer(const SocketAddress& address);
    ~TcpSingleServer() override;

    void send(const QByteArray& array) override;

private:
    std::unique_ptr<QTcpServer> m_server;
    std::shared_ptr<TcpMessenger> m_messenger;
};

} // namespace rpc
