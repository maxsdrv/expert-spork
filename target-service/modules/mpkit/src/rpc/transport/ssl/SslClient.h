#pragma once

#include "rpc/transport/Socket.h"
#include "rpc/transport/Transport.h"

#include <QQueue>
#include <QSslError>

class QByteArray;
class QSslSocket;

namespace rpc
{

struct SslData;
class TcpMessenger;

class SslClient : public Transport
{

public:
    SslClient(
        SocketAddress address,
        const SslData& sslData,
        const QList<QSslError::SslError>& ignoredSslErrors = QList<QSslError::SslError>());
    ~SslClient() override;

    void send(const QByteArray& array) override;

private:
    void establishConnection();

private:
    SocketAddress m_address;
    QQueue<QByteArray> m_queue;
    std::unique_ptr<QSslSocket> m_socket;
    std::shared_ptr<TcpMessenger> m_messenger;
};

} // namespace rpc
