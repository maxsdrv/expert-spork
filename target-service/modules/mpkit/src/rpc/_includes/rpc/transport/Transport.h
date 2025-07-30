#pragma once

#include "boost/signals2.h"

#include <memory>

class QByteArray;

namespace rpc
{

class IncomingMessage;

class Transport
{
    ADD_SIGNAL(Received, void(std::shared_ptr<IncomingMessage>))

    virtual ~Transport() = default;

public:
    virtual void send(const QByteArray& array) = 0;
};

} // namespace rpc
