#pragma once

#include "rpc/transport/Messenger.h"
#include "rpc/transport/Transport.h"

#include <QByteArray>

#include <memory>

namespace rpc
{

class TransportMockMessenger: public Messenger
{
public:
    explicit TransportMockMessenger(QByteArray* buffer);
    void send(const QByteArray& array) noexcept override;

private:
    QByteArray* m_buffer;
};

class TransportMock : public Transport
{
public:
    TransportMock();
    void send(const QByteArray& array) override;
    void receive(const QByteArray& array);
    QByteArray buffer() const;

private:
    std::shared_ptr<Messenger> m_messenger;
    QByteArray m_buffer;
};

} // namespace rpc
