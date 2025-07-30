#include "SerialEndpoint.h"

#include "SerialMessenger.h"

#include "transport/IncomingMessageImpl.h"

#include "rpc/transport/TransportExceptions.h"

namespace rpc
{

SerialEndpoint::SerialEndpoint(
    const QString& name,
    QSerialPort::BaudRate baudRate,
    QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity,
    QSerialPort::StopBits stopBits) :
  m_port(std::make_unique<QSerialPort>(name)),
  m_messenger(std::make_shared<SerialMessenger>(m_port.get()))
{
    QObject::connect(
        m_messenger.get(),
        &SerialMessenger::received,
        m_messenger.get(),
        [this](const auto& buffer)
        { m_Received(std::make_shared<IncomingMessageImpl>(buffer, m_messenger)); });

    m_port->setBaudRate(baudRate);
    m_port->setDataBits(dataBits);
    m_port->setParity(parity);
    m_port->setStopBits(stopBits);

    if (!m_port->open(QIODevice::ReadWrite))
    {
        BOOST_THROW_EXCEPTION(
            exception::TransportCreationFailed(m_port->errorString().toStdString()));
    }
}

SerialEndpoint::~SerialEndpoint() = default;

void SerialEndpoint::send(const QByteArray& array)
{
    m_messenger->send(array);
}

} // namespace rpc
