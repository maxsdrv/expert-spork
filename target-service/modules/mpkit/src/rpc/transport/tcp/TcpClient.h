#pragma once

#include "rpc/transport/Transport.h"

#include "rpc/transport/Socket.h"

#include <memory>

#include <QQueue>

class QByteArray;
class QTcpSocket;

namespace rpc
{

class TcpMessenger;

class TcpClient : public Transport
{

public:
    explicit TcpClient(SocketAddress address);
    ~TcpClient() override;

    void send(const QByteArray& array) override;

private:
    SocketAddress m_address;
    QQueue<QByteArray> m_queue;
    std::unique_ptr<QTcpSocket> m_socket;
    std::shared_ptr<TcpMessenger> m_messenger;
};

} // namespace rpc
