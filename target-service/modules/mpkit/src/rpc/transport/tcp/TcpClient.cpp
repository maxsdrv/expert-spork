#include "TcpClient.h"

#include "TcpMessenger.h"

#include "transport/IncomingMessageImpl.h"
#include "transport/Socket.h"

#include "rpc/transport/IncomingMessage.h"

#include <QTcpSocket>

namespace rpc
{

TcpClient::TcpClient(SocketAddress address) :
  m_address(std::move(address)),
  m_socket(std::make_unique<QTcpSocket>()),
  m_messenger(std::make_shared<TcpMessenger>(m_socket.get()))
{
    QObject::connect(
        m_messenger.get(),
        &TcpMessenger::received,
        m_messenger.get(),
        [this](const auto& buffer)
        { m_Received(std::make_shared<IncomingMessageImpl>(buffer, m_messenger)); });

    QObject::connect(
        m_socket.get(),
        &QAbstractSocket::stateChanged,
        m_socket.get(),
        [this](auto state)
        {
            if (state == QAbstractSocket::ConnectedState)
            {
                while (!m_queue.isEmpty())
                {
                    m_messenger->send(m_queue.dequeue());
                }
            }
            else if (state == QAbstractSocket::UnconnectedState)
            {
                m_queue.clear();
            }
        });
}

TcpClient::~TcpClient() = default;

void TcpClient::send(const QByteArray& array)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_messenger->send(array);
        return;
    }

    if (m_socket->state() == QAbstractSocket::UnconnectedState)
    {
        m_socket->connectToHost(m_address.host, m_address.port);
    }

    m_queue.enqueue(array);
}

} // namespace rpc
