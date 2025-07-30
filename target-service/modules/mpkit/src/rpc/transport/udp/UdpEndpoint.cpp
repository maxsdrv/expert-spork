#include "UdpEndpoint.h"

#include "UdpMessenger.h"

#include "transport/IncomingMessageImpl.h"
#include "transport/Socket.h"

#include "rpc/transport/TransportExceptions.h"

#include <QUdpSocket>

namespace rpc
{

UdpEndpoint::UdpEndpoint(SocketAddress destination, const SocketAddress& listen) :
  m_socket(std::make_unique<QUdpSocket>()),
  m_messenger(std::make_shared<UdpMessenger>(m_socket.get(), std::move(destination)))
{
    QObject::connect(
        m_messenger.get(),
        &UdpMessenger::received,
        m_messenger.get(),
        [this](const auto& buffer)
        { m_Received(std::make_shared<IncomingMessageImpl>(buffer, m_messenger)); });

    if (!m_socket->bind(listen.host, listen.port))
    {
        BOOST_THROW_EXCEPTION(
            exception::TransportCreationFailed(m_socket->errorString().toStdString()));
    }
}

UdpEndpoint::~UdpEndpoint() = default;

void UdpEndpoint::send(const QByteArray& array)
{
    m_messenger->send(array);
}

} // namespace rpc
