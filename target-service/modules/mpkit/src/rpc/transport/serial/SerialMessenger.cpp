#include "SerialMessenger.h"

#include <QSerialPort>

namespace rpc
{

namespace
{

const quint16 headerSync = 0xFFFF;

} // namespace

SerialMessenger::SerialMessenger(gsl::not_null<QSerialPort*> port, QObject* parent) :
  Messenger(parent), m_wrapper(headerSync), m_port(port)
{
    QObject::connect(
        m_port.get(),
        &QSerialPort::readyRead,
        this,
        [this]()
        {
            auto messages = m_wrapper.unwrap(m_port->readAll());
            for (const auto& message: messages)
            {
                emit received(message);
            }
        });
}

void SerialMessenger::send(const QByteArray& array) noexcept
{
    auto wrapped = m_wrapper.wrap(array);
    if (wrapped.size() > 0)
    {
        m_port->write(wrapped);
        m_port->waitForBytesWritten();
    }
}

} // namespace rpc
