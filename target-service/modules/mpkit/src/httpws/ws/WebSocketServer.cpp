#include "httpws/ws/WebSocketServer.h"

#include "httpws/NetworkError.h"

#include "json/JsonUtilities.h"

#include "exception/KeyInfo.h"
#include "exception/MissingEntity.h"

#include "log/Log2.h"
#include "qttypes/QStringConverters.h"

#include <QMetaObject>
#include <QWebSocket>
#include <QWebSocketServer>

namespace http
{

namespace
{

QString socketIdentifier(QWebSocket* peer)
{
    return QStringLiteral("%1:%2").arg(
        peer->peerAddress().toString(), QString::number(peer->peerPort()));
}

QHostAddress fromString(const QString& str)
{
    if (str == "any")
    {
        return QHostAddress::Any;
    }
    if (str == "localhost")
    {
        return QHostAddress::LocalHost;
    }
    return QHostAddress(str);
}

} // namespace

WebSocketServer::WebSocketServer(ConnectionSettings config, QObject* parent) :
  QObject(parent),
  m_config(std::move(config)),
  m_server(
      new QWebSocketServer(QStringLiteral("RCT server"), QWebSocketServer::NonSecureMode, this))
{
    qRegisterMetaType<QWebSocket*>("QWebSocket*");
    qRegisterMetaType<PeerId>("http::WebSocketServer::PeerId");

    moveToThread(&m_thread);
    m_thread.start();
}

WebSocketServer::~WebSocketServer()
{
    if (m_thread.isRunning())
    {
        m_thread.quit();
        m_thread.wait();
    }
}

void WebSocketServer::listen()
{
    if (thread() != QThread::currentThread())
    {
        QMetaObject::invokeMethod(
            this,                  // NOLINT
            [this] { listen(); },  // NOLINT
            Qt::QueuedConnection); // NOLINT
        return;
    }

    bool result = m_server->listen(fromString(m_config.host), m_config.port);
    if (result)
    {
        LOG_INFO << "Websocket server listening on: " << m_config.host << ":" << m_config.port;
        connect(
            m_server, &QWebSocketServer::newConnection, this, &WebSocketServer::saveConnection);
        connect(m_server, &QWebSocketServer::closed, this, &WebSocketServer::dropConnections);
    }
    else
    {
        auto error = m_server->error();
        auto description = strings::toUtf8(m_server->errorString());
        LOG_WARNING << "Websocket server listen failed with " << error << " (" << description
                    << ")";
        BOOST_THROW_EXCEPTION(exception::NetworkError() << exception::NetworkInfo(description));
    }
}

void WebSocketServer::broadcast(const QJsonObject& object)
{
    QMetaObject::invokeMethod(
        this, "sendObject", Qt::QueuedConnection, Q_ARG(QJsonObject, object));
}

void WebSocketServer::send(const PeerId& peerId, const QJsonObject& object)
{
    auto socketIt = m_connections.find(peerId);
    if (socketIt != m_connections.end())
    {
        QMetaObject::invokeMethod(
            this,
            "sendObject",
            Qt::QueuedConnection,
            Q_ARG(QWebSocket*, socketIt->second),
            Q_ARG(QJsonObject, object));
    }
    else
    {
        BOOST_THROW_EXCEPTION(
            exception::MissingEntity() << exception::toKeyInfo(strings::toUtf8(peerId)));
    }
}

void WebSocketServer::sendObject(const QJsonObject& object)
{
    for (const auto& socket: m_connections)
    {
        sendObject(socket.second, object);
    }
}

void WebSocketServer::sendObject(QWebSocket* socket, const QJsonObject& object)
{
    socket->sendBinaryMessage(json::toByteArray(object, QJsonDocument::Compact));
}

void WebSocketServer::saveConnection()
{
    auto* socket = m_server->nextPendingConnection();
    socket->setParent(this);
    auto peerId = socketIdentifier(socket);
    LOG_INFO << "Websocket client connected from: " << peerId;
    connect(socket, &QWebSocket::disconnected, this, &WebSocketServer::removeSocket);
    m_connections.insert({peerId, socket});
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    // QWebSocket in older Qt versions has problems with implicit flushing, it
    // causes 'timeout error' on client side. Therefore we force explicit
    // flushing at each writing. This behavior has been fixed in new Qt
    // versions (5.14.0 and newer)
    connect(socket, &QWebSocket::bytesWritten, socket, &QWebSocket::flush);
#endif // QT_VERSION
    socket->setParent(this);
    emit clientConnected(peerId);
}

void WebSocketServer::dropConnections()
{
    m_connections.clear();
}

void WebSocketServer::removeSocket()
{
    if (auto* socket = qobject_cast<QWebSocket*>(sender()))
    {
        auto peerId = socketIdentifier(socket);
        LOG_INFO << "Websocket client disconnected: " << peerId;
        m_connections.erase(peerId);
        socket->deleteLater();
        emit clientDisconnected(peerId);
    }
}

} // namespace http
