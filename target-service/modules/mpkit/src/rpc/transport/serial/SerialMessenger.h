#pragma once

#include "rpc/transport/Messenger.h"
#include "rpc/transport/messages/HeaderMessageWrapper.h"

#include "gsl/pointers"

class QSerialPort;

namespace rpc
{

class SerialMessenger : public Messenger
{
    Q_OBJECT

public:
    explicit SerialMessenger(gsl::not_null<QSerialPort*> port, QObject* parent = nullptr);
    void send(const QByteArray& array) noexcept override;

private:
    HeaderMessageWrapper<SyncChecksumLengthBEHeader<quint16, quint16>> m_wrapper;
    gsl::not_null<QSerialPort*> m_port;
};

} // namespace rpc
