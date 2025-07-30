#include "TcpMultiServer.h"

#include "TcpMessenger.h"

#include "transport/IncomingMessageImpl.h"
#include "transport/Socket.h"

#include "rpc/transport/TransportExceptions.h"

#include <QTcpServer>
#include <QTcpSocket>

namespace rpc
{

TcpMultiServer::TcpMultiServer(const SocketAddress& address) :
  m_server(std::make_unique<QTcpServer>())
{
    if (address.host.isNull())
    {
        BOOST_THROW_EXCEPTION(exception::TransportCreationFailed("Invalid address"));
    }

    if (!m_server->listen(address.host, address.port))
    {
        BOOST_THROW_EXCEPTION(
            exception::TransportCreationFailed(m_server->errorString().toStdString()));
    }

    QObject::connect(
        m_server.get(),
        &QTcpServer::newConnection,
        m_server.get(),
        [this]
        {
            auto* socket = m_server->nextPendingConnection();
            auto messenger = std::make_shared<TcpMessenger>(socket);
            auto peerAddr = SocketAddress{socket->peerAddress(), socket->peerPort()};
            m_messengers.insert({peerAddr, messenger});

            QObject::connect(
                messenger.get(),
                &TcpMessenger::received,
                m_server.get(),
                [this, messenger](const auto& buffer)
                { m_Received(std::make_shared<IncomingMessageImpl>(buffer, messenger)); });

            QObject::connect(
                socket,
                &QAbstractSocket::disconnected,
                m_server.get(),
                [this, peerAddr]() { m_messengers.erase(peerAddr); });
        });
}

TcpMultiServer::~TcpMultiServer() = default;

void TcpMultiServer::send(const QByteArray& /*array*/)
{
    BOOST_THROW_EXCEPTION(exception::TransportMethodNotSupported("TcpMultiServer::send"));
}

} // namespace rpc
