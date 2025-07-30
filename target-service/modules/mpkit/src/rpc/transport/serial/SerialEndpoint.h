#pragma once

#include "rpc/transport/Transport.h"

#include <QSerialPort>

#include <memory>

namespace rpc
{

class SerialMessenger;

class SerialEndpoint : public Transport
{
public:
    SerialEndpoint(
        const QString& name,
        QSerialPort::BaudRate baudRate,
        QSerialPort::DataBits dataBits,
        QSerialPort::Parity parity,
        QSerialPort::StopBits stopBits);

    ~SerialEndpoint() override;

    void send(const QByteArray& array) override;

private:
    void readData();

private:
    std::unique_ptr<QSerialPort> m_port;
    std::shared_ptr<SerialMessenger> m_messenger;
};

} // namespace rpc
