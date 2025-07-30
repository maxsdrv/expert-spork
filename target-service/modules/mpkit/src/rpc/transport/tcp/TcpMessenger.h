#pragma once

#include "rpc/transport/Messenger.h"
#include "rpc/transport/messages/HeaderMessageWrapper.h"

#include "gsl/pointers"

class QTcpSocket;

namespace rpc
{

class TcpMessenger : public Messenger
{
    Q_OBJECT

public:
    explicit TcpMessenger(gsl::not_null<QTcpSocket*> socket, QObject* parent = nullptr);
    void send(const QByteArray& array) noexcept override;

protected:
    const QTcpSocket* socket() const noexcept;

private:
    using Wrapper = HeaderMessageWrapper<SyncChecksumLengthBEHeader<quint16, quint32>>;
    Wrapper m_wrapper;
    gsl::not_null<QTcpSocket*> m_socket;
};

} // namespace rpc
