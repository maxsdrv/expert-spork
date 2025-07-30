#pragma once

#include <QJsonObject>

#include <functional>

namespace mpk::rest
{

class MessageSubscriber
{
public:
    using MessageHandler = std::function<void(const QJsonObject& object)>;

    virtual ~MessageSubscriber() = default;

    virtual void subscribe(const QString& messageType, MessageHandler messageHandler) const = 0;
    virtual void unsubscribe(const QString& messageType) const = 0;
    // TODO: Add subscribeAll(), unsubscribeAll()
};

} // namespace mpk::rest
