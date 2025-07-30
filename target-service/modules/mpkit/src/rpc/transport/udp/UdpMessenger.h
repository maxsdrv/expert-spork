#pragma once

#include "rpc/transport/Messenger.h"
#include "rpc/transport/Socket.h"

#include "gsl/pointers"

class QUdpSocket;

namespace rpc
{

class UdpMessenger : public Messenger
{
    Q_OBJECT

public:
    UdpMessenger(
        gsl::not_null<QUdpSocket*> socket, SocketAddress destination, QObject* parent = nullptr);
    void send(const QByteArray& array) noexcept override;

private:
    gsl::not_null<QUdpSocket*> m_socket;
    SocketAddress m_destination;
};

} // namespace rpc
