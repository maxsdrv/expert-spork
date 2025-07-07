#include "WsPublisher.h"

#include "dss-core/audit/AuditEventLogger.h"

#include "httpws/ws/WebSocketServer.h"
#include "log/Log2.h"
#include "mpk/rest/HttpWsConnection.h"

namespace mpk::dss::core
{

namespace
{

constexpr auto messageTag = "message";
constexpr auto dataTag = "data";

} // namespace

WsPublisher::WsPublisher(
    http::ConnectionSettings connectionSettings, QObject* parent) :
  QObject(parent),
  m_server{
      std::make_unique<http::WebSocketServer>(std::move(connectionSettings))}
{
    connect(
        m_server.get(),
        &http::WebSocketServer::clientConnected,
        this,
        [](const auto& id) { core::auditLogger().logWsClientConnected(id); });
    connect(
        m_server.get(),
        &http::WebSocketServer::clientDisconnected,
        this,
        [](const auto& id)
        { core::auditLogger().logWsClientDisconnected(id); });
}

WsPublisher::~WsPublisher() = default;

void WsPublisher::run()
{
    LOG_INFO << "Websocket publisher: run";
    m_server->listen();
}

void WsPublisher::sendNotification(
    const QString& /*domain*/, const QString& message, const QJsonObject& data)
{
    // TODO: check for domain subscriptions
    // use WebSocketServer::send

    LOG_DEBUG << "Websocket publisher: notification message: " << message;
    LOG_TRACE << "Websocket publisher: notification data: " << data;

    auto json = QJsonObject{};
    json[messageTag] = message;
    json[dataTag] = data;

    m_server->broadcast(json);
}

} // namespace mpk::dss::core
