#pragma once

#include "WebSocketClient.h"

#include <QObject>

#include <map>

class QWebSocket;

namespace http
{

class WebSocketClientImpl : public QObject, public WebSocketClient
{
    Q_OBJECT

public:
    explicit WebSocketClientImpl(
        QWebSocket* socket, QObject* parent = nullptr, bool handleAllMessages = true);

    void registerHandler(MessageHandler handler) override;
    void registerHandler(const QString& type, MessageHandler handler) override;

    void unregisterHandler(const QString& type) override;
    void unregisterHandler() override;

private:
    void handleMessage(const QByteArray& message);
    void handleJson(const QJsonObject& json);

private:
    QWebSocket* m_socket;
    std::map<QString, MessageHandler> m_handlers;
    MessageHandler m_handlerAllMessages;

    bool m_handleAllMessages;
};

} // namespace http
