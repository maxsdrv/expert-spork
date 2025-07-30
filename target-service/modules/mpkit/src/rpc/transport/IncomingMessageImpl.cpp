#include "IncomingMessageImpl.h"

#include "Messenger.h"

#include "rpc/transport/TransportExceptions.h"

namespace rpc
{

IncomingMessageImpl::IncomingMessageImpl(QByteArray buffer, std::weak_ptr<Messenger> messenger) :
  m_buffer(std::move(buffer)), m_messenger(std::move(messenger))
{
}

[[nodiscard]] QByteArray const& IncomingMessageImpl::buffer() const noexcept
{
    return m_buffer;
}

void IncomingMessageImpl::reply(const QByteArray& array)
{
    if (auto messenger = m_messenger.lock())
    {
        messenger->send(array);
    }
    else
    {
        BOOST_THROW_EXCEPTION(exception::TransportException("Transport messenger lost"));
    }
}

} // namespace rpc
