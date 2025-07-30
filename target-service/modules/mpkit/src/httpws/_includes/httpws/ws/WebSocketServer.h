#pragma once

#include "httpws/ConnectionSettings.h"

#include <QObject>
#include <QThread>

class QWebSocket;
class QWebSocketServer;

namespace http
{

/**
 * Simple web socket server. Handle incoming connections
 */
class WebSocketServer : public QObject
{
    Q_OBJECT

public:
    using PeerId = QString;
    explicit WebSocketServer(ConnectionSettings config, QObject* parent = nullptr);
    ~WebSocketServer() override;

    /**
     * Start listening
     */
    void listen();

    /**
     * Send @a object to all connected clients
     */
    void broadcast(const QJsonObject& object);

    /**
     * Send @a object to selected client
     */
    void send(const PeerId& peerId, const QJsonObject& object);

signals:
    void clientConnected(http::WebSocketServer::PeerId id);
    void clientDisconnected(http::WebSocketServer::PeerId id);

private slots:
    /**
     * Send @a object to all connected clients
     */
    void sendObject(const QJsonObject& object);
    static void sendObject(QWebSocket* socket, const QJsonObject& object);
    void saveConnection();
    void dropConnections();
    void removeSocket();

private:
    QThread m_thread;
    ConnectionSettings m_config;

    QWebSocketServer* m_server;
    std::map<PeerId, QWebSocket*> m_connections;
};

} // namespace http
