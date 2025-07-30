#pragma once

#include "httpws/ConnectionSettings.h"
#include "httpws/Token.h"

#include "mpk/rest/ConnectionState.h"
#include "mpk/rest/OperationResult.h"

#include <QObject>
#include <QTimer>

class QWebSocket;

namespace http
{

class AuthHttpClient;
struct ConnectionSettings;
class HttpClient;
class WebSocketClient;

} // namespace http

namespace mpk::rest
{

// TODO: A duplicate of the rctcore::DeviceConnection,
// take it to the common library.
// TODO: This class is bloated. Consider refactoring.
class HttpWsConnection final : public QObject
{
    Q_OBJECT

public:
    using State = ConnectionState::Value;
    HttpWsConnection(
        http::ConnectionSettings httpSettings,
        http::ConnectionSettings webSocketSettings,
        QString name,
        QObject* parent = nullptr);

    void setHttpSettings(http::ConnectionSettings httpSettings);
    http::ConnectionSettings httpSettings() const;
    void setWebSocketSettings(http::ConnectionSettings webSocketSettings);
    http::ConnectionSettings webSocketSettings() const;

    http::ConnectionSettings localWebSocketSettings() const;

    const http::Token& accessToken() const;
    void setAccessToken(const http::Token& accessToken);

    http::HttpClient& httpClient() const;
    http::WebSocketClient& webSocketClient() const;
    State state() const;

    void reportOperationResult(const QString& operation, OperationResult::Value code) const;

public slots:
    void start(bool timeout = false);
    void stop();
    void restart(bool timeout = false);

signals:
    void stateChanged(HttpWsConnection::State state);
    void httpSettingsChanged(http::ConnectionSettings);
    void webSocketSettingsChanged(http::ConnectionSettings);
    void operationResultReceived( // clazy:exclude=const-signal-or-slot
        const QString& operation,
        OperationResult::Value code) const;

private:
    std::string url() const;
    void tryRestart(bool timeout);
    void closeWS();
    void infoRequest();
    bool setState(State state);

    enum class ConnectionStep : std::uint8_t;
    void scheduleReset();
    void scheduleStep(std::chrono::seconds timeout, ConnectionStep step);
    void scheduleStep(ConnectionStep step);

private slots:
    void makeStep();
    void openWS();

private:
    enum class ConnectionStep : std::uint8_t
    {
        OFF,
        TIMEOUT,
        INFO_REQUEST,
        WEBSOCKET
    };

    http::ConnectionSettings m_httpSettings;
    http::ConnectionSettings m_webSocketSettings;
    QString m_name;

    http::HttpClient* m_httpClient;
    QWebSocket* m_webSocket;
    http::WebSocketClient* m_webSocketClient;

    QTimer m_pingTimer;
    QTimer m_pongTimer;
    State m_state = State::DISCONNECTED;

    ConnectionStep m_connectionStep = ConnectionStep::OFF;
    QTimer m_stepTimer;

    http::Token m_accessToken;
};

} // namespace mpk::rest
