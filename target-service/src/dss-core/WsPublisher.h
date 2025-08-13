#pragma once

#include "gsl/pointers"

#include <QObject>

namespace http
{

struct ConnectionSettings;
class WebSocketServer;

} // namespace http

namespace mpk::dss::core
{

class WsPublisher : public QObject
{
    Q_OBJECT

public:
    explicit WsPublisher(
        http::ConnectionSettings connectionSettings, QObject* parent = nullptr);
    ~WsPublisher() override;

    void run();

public slots:
    void sendNotification(
        const QString& domain, const QString& message, const QJsonObject& data);

private:
    std::unique_ptr<http::WebSocketServer> m_server;
};

} // namespace mpk::dss::core
