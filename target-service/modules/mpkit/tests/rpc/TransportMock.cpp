#include "TransportMock.h"

#include "rpc/transport/IncomingMessageImpl.h"

namespace rpc
{

TransportMockMessenger::TransportMockMessenger(QByteArray* buffer):
    m_buffer(buffer)
{
}

void TransportMockMessenger::send(const QByteArray& array) noexcept
{
    *m_buffer = array;
}

TransportMock::TransportMock():
    m_messenger(std::make_shared<TransportMockMessenger>(&m_buffer))
{
}

void TransportMock::send(const QByteArray& array)
{
    m_buffer = array;
}

void TransportMock::receive(const QByteArray& array)
{
    m_Received(std::make_shared<IncomingMessageImpl>(array, m_messenger));
}

QByteArray TransportMock::buffer() const
{
    return m_buffer;
}

} // namespace rpc
