#pragma once

#include "rpc/transport/Transport.h"

#include <memory>

class QUdpSocket;

namespace rpc
{

struct SocketAddress;
class UdpMessenger;

class UdpEndpoint : public Transport
{
public:
    UdpEndpoint(SocketAddress destination, const SocketAddress& listen);
    ~UdpEndpoint() override;

    void send(const QByteArray& array) override;

private:
    std::unique_ptr<QUdpSocket> m_socket;
    std::shared_ptr<UdpMessenger> m_messenger;
};

} // namespace rpc
