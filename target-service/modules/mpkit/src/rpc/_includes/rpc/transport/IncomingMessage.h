#pragma once

#include <QByteArray>

namespace rpc
{

class IncomingMessage
{
public:
    virtual ~IncomingMessage() = default;
    [[nodiscard]] virtual const QByteArray& buffer() const noexcept = 0;
    virtual void reply(const QByteArray& buffer) = 0;
};

} // namespace rpc
