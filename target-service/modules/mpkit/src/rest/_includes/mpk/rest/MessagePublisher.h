#pragma once

#include <QJsonObject>

namespace mpk::rest
{

class MessagePublisher
{
public:
    virtual ~MessagePublisher() = default;

    virtual void broadcast(const QJsonObject& message) const = 0;
};

} // namespace mpk::rest
