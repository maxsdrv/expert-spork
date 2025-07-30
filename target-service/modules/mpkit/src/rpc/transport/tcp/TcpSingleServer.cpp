#include "TcpSingleServer.h"

#include "TcpMessenger.h"

#include "transport/IncomingMessageImpl.h"
#include "transport/Socket.h"

#include "rpc/transport/TransportExceptions.h"

#include <QTcpServer>
#include <QTcpSocket>

namespace rpc
{

TcpSingleServer::TcpSingleServer(const SocketAddress& address) :
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
            m_messenger = std::make_shared<TcpMessenger>(socket);

            QObject::connect(
                m_messenger.get(),
                &TcpMessenger::received,
                m_server.get(),
                [this](const auto& buffer)
                { m_Received(std::make_shared<IncomingMessageImpl>(buffer, m_messenger)); });

            QObject::connect(
                socket,
                &QAbstractSocket::disconnected,
                m_server.get(),
                [this]() { m_messenger.reset(); });
        });
}

TcpSingleServer::~TcpSingleServer() = default;

void TcpSingleServer::send(const QByteArray& array)
{
    if (!m_messenger)
    {
        BOOST_THROW_EXCEPTION(exception::TransportNotConnected("Awaiting for connection"));
    }

    m_messenger->send(array);
}

} // namespace rpc
