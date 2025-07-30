#include "TcpMessenger.h"

#include "rpc/transport/TransportExceptions.h"

#include <QTcpSocket>

namespace rpc
{

namespace
{

const quint16 headerSync = 0xFFFF;

} // namespace

TcpMessenger::TcpMessenger(gsl::not_null<QTcpSocket*> socket, QObject* parent) :
  Messenger(parent), m_wrapper(headerSync), m_socket(socket)
{
    connect(
        m_socket.get(),
        &QTcpSocket::readyRead,
        this,
        [this]()
        {
            while (m_socket->bytesAvailable() > 0)
            {
                auto messages = m_wrapper.unwrap(m_socket->readAll());
                for (const auto& message: messages)
                {
                    emit received(message);
                }
            }
        });
}

void TcpMessenger::send(const QByteArray& array) noexcept
{
    auto wrapped = m_wrapper.wrap(array);
    if (wrapped.size() > 0)
    {
        m_socket->write(wrapped);
    }
};

const QTcpSocket* TcpMessenger::socket() const noexcept
{
    return m_socket.get();
}

} // namespace rpc
