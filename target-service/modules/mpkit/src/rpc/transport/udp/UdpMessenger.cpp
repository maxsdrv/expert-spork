#include "UdpMessenger.h"

#include "rpc/transport/TransportExceptions.h"

#include <QNetworkDatagram>
#include <QUdpSocket>

namespace rpc
{

UdpMessenger::UdpMessenger(
    gsl::not_null<QUdpSocket*> socket, SocketAddress destination, QObject* parent) :
  Messenger(parent), m_socket(socket), m_destination(std::move(destination))
{
    if (m_destination.host.isNull())
    {
        BOOST_THROW_EXCEPTION(exception::TransportCreationFailed("Invalid destination address"));
    }

    connect(
        m_socket.get(),
        &QUdpSocket::readyRead,
        this,
        [this]()
        {
            while (m_socket->hasPendingDatagrams())
            {
                auto datagram = m_socket->receiveDatagram();
                emit received(datagram.data());
            }
        });
}

void UdpMessenger::send(const QByteArray& array) noexcept
{
    QNetworkDatagram datagram{array, m_destination.host, m_destination.port};
    m_socket->writeDatagram(datagram);
};

} // namespace rpc
