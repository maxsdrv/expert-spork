#pragma once

#include "rpc/transport/Transport.h"

#include <memory>
#include <vector>

class QTcpServer;

namespace rpc
{

struct SocketAddress;
class TcpMessenger;

class TcpMultiServer : public Transport
{

public:
    explicit TcpMultiServer(const SocketAddress& address);
    ~TcpMultiServer() override;

    void send(const QByteArray& array) override;

private:
    std::unique_ptr<QTcpServer> m_server;
    std::map<SocketAddress, std::shared_ptr<TcpMessenger>> m_messengers;
};

} // namespace rpc
