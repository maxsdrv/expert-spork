#include "rpc/transport/IncomingMessage.h"

#include <memory>

namespace rpc
{

class Messenger;

class IncomingMessageImpl : public IncomingMessage
{
public:
    IncomingMessageImpl(QByteArray buffer, std::weak_ptr<Messenger> messenger);

    [[nodiscard]] QByteArray const& buffer() const noexcept override;
    void reply(const QByteArray& array) override;

private:
    QByteArray m_buffer;
    std::weak_ptr<Messenger> m_messenger;
};

} // namespace rpc
