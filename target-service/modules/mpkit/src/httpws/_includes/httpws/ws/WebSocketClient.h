#pragma once

#include <functional>

#include <QJsonObject>
#include <QString>

namespace http
{

class WebSocketClient
{
public:
    using MessageHandler = std::function<void(const QJsonObject& json)>;

    virtual ~WebSocketClient() = default;

    virtual void registerHandler(MessageHandler handler) = 0;
    virtual void registerHandler(const QString& type, MessageHandler handler) = 0;

    virtual void unregisterHandler(const QString& type) = 0;
    virtual void unregisterHandler() = 0;
};

} // namespace http
