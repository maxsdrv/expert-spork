#include "mpk/rest/HttpWsConnection.h"

#include "httpws/AuthHttpClient.h"
#include "httpws/HttpClient.h"
#include "httpws/ws/WebSocketClientImpl.h"

#include "log/Log2.h"

#include "qt/MetaEnum.h"
#include "qt/Strings.h"

#include <QWebSocket>

namespace
{

const auto pingInterval = std::chrono::seconds(5);
const auto pongInterval = std::chrono::seconds(4);
const auto reconnectInterval = std::chrono::seconds(5);

constexpr auto maxThreadsCount = 2;

QUrl createWebSocketUrl(const http::ConnectionSettings& settings)
{
    return QUrl("ws://" + settings.host + ":" + QString::number(settings.port));
}

} // namespace

namespace mpk::rest
{

HttpWsConnection::HttpWsConnection(
    http::ConnectionSettings httpSettings,
    http::ConnectionSettings webSocketSettings,
    QString name,
    QObject* parent) :
  QObject(parent),
  m_httpSettings(std::move(httpSettings)),
  m_webSocketSettings(std::move(webSocketSettings)),
  m_name(std::move(name)),
  m_httpClient(new http::HttpClient(m_httpSettings, maxThreadsCount, this)),
  m_webSocket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this)),
  m_webSocketClient(new http::WebSocketClientImpl(m_webSocket, this, false))
{
    QObject::connect(
        m_webSocket,
        &QWebSocket::connected,
        this,
        [this]()
        {
            LOG_INFO << "Connection " << m_name << " websocket connected to " << url();
            m_pingTimer.start();
            setState(State::CONNECTED);
        });
    QObject::connect(
        m_webSocket,
        &QWebSocket::disconnected,
        this,
        [this]()
        {
            LOG_INFO << "Connection " << m_name << " websocket disconnected from " << url();
            tryRestart(true);
        });
    QObject::connect(
        m_webSocket,
#if QT_MAJOR < 6
        QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
#else
        &QWebSocket::errorOccurred,
#endif
        this,
        [this](QAbstractSocket::SocketError /*error*/)
        {
            LOG_INFO << "Connection " << m_name
                     << " websocket error: " << strings::toUtf8(m_webSocket->errorString())
                     << " occurred for " << url();
            tryRestart(true);
        });
    QObject::connect(
        m_webSocket,
        &QWebSocket::pong,
        this,
        [this](quint64 elapsedTime)
        {
            LOG_TRACE << "Connection " << m_name << " pong " << url() << " in " << elapsedTime
                      << " ms";
            m_pongTimer.stop();
        });

    m_pingTimer.setInterval(pingInterval);
    QObject::connect(
        &m_pingTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            LOG_TRACE << "Connection " << m_name << " ping " << url();
            m_webSocket->ping();
            m_pongTimer.start();
        });
    m_pongTimer.setInterval(pongInterval);
    QObject::connect(
        &m_pongTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            LOG_ERROR << "Connection " << m_name << " pong timeout " << url();
            restart();
        });

    QObject::connect(&m_stepTimer, &QTimer::timeout, this, &HttpWsConnection::makeStep);
}

void HttpWsConnection::setHttpSettings(http::ConnectionSettings httpSettings)
{
    if (m_httpSettings != httpSettings)
    {
        m_httpSettings = std::move(httpSettings);
        m_httpClient->setSettings(m_httpSettings);
        emit httpSettingsChanged(m_httpSettings);
    }
}

http::ConnectionSettings HttpWsConnection::httpSettings() const
{
    return m_httpSettings;
}

void HttpWsConnection::setWebSocketSettings(http::ConnectionSettings webSocketSettings)
{
    if (m_webSocketSettings != webSocketSettings)
    {
        m_webSocketSettings = std::move(webSocketSettings);
        emit webSocketSettingsChanged(m_webSocketSettings);
        restart();
    }
}

http::ConnectionSettings HttpWsConnection::webSocketSettings() const
{
    return m_webSocketSettings;
}

http::ConnectionSettings HttpWsConnection::localWebSocketSettings() const
{
    return {.host = m_webSocket->localAddress().toString(), .port = m_webSocket->localPort()};
}

const http::Token& HttpWsConnection::accessToken() const
{
    return m_accessToken;
}

void HttpWsConnection::setAccessToken(const http::Token& accessToken)
{
    m_accessToken = accessToken;
    m_httpClient->setAuthToken(accessToken.value);
}

http::HttpClient& HttpWsConnection::httpClient() const
{
    return *m_httpClient;
}

http::WebSocketClient& HttpWsConnection::webSocketClient() const
{
    return *m_webSocketClient;
}

ConnectionState::Value HttpWsConnection::state() const
{
    return m_state;
}

void HttpWsConnection::reportOperationResult(
    const QString& operation, OperationResult::Value code) const
{
    emit operationResultReceived(operation, code);
}

void HttpWsConnection::start(bool timeout)
{
    if (m_connectionStep == ConnectionStep::OFF)
    {
        scheduleStep(timeout ? ConnectionStep::TIMEOUT : ConnectionStep::INFO_REQUEST);
        // TODO: Some intermediate state may be changed here
    }
}

void HttpWsConnection::stop()
{
    closeWS();
    m_stepTimer.stop();
    m_connectionStep = ConnectionStep::OFF;
    setState(ConnectionState::Value::DISCONNECTED);
}

void HttpWsConnection::restart(bool timeout)
{
    stop();
    start(timeout);
}

std::string HttpWsConnection::url() const
{
    return strings::toUtf8(m_webSocket->requestUrl().toString());
}

void HttpWsConnection::tryRestart(bool timeout)
{
    if (setState(ConnectionState::Value::DISCONNECTED))
    {
        restart(timeout);
    }
}

void HttpWsConnection::closeWS()
{
    m_webSocket->close();
    m_pingTimer.stop();
    m_pongTimer.stop();
}

void HttpWsConnection::infoRequest()
{
    if (!http::ConnectionSettings::isValid(m_httpSettings))
    {
        LOG_WARNING << "Connection " << m_name << " address is not set";
        stop();
    }

    setState(State::CONNECTING);
    // TODO: will be implemented later

    scheduleStep(ConnectionStep::WEBSOCKET);
}

void HttpWsConnection::openWS()
{
    auto url = createWebSocketUrl(webSocketSettings());
    LOG_INFO << "Connection " << m_name << " trying to open websocket connection to "
             << url.toString();
    m_webSocket->open(url);
}

bool HttpWsConnection::setState(HttpWsConnection::State state)
{
    if (m_state != state)
    {
        m_state = state;
        LOG_DEBUG << "Connection " << m_name << " state set to " << qt::metaEnumToString(state);
        emit stateChanged(m_state);
        return true;
    }
    return false;
}

void HttpWsConnection::scheduleStep(std::chrono::seconds timeout, ConnectionStep step)
{
    m_connectionStep = step;
    m_stepTimer.setInterval(timeout);
    m_stepTimer.start();
}

void HttpWsConnection::scheduleStep(ConnectionStep step)
{
    m_connectionStep = step;
    m_stepTimer.setInterval(0);
    m_stepTimer.start();
}

void HttpWsConnection::makeStep()
{
    m_stepTimer.stop();
    switch (m_connectionStep)
    {
        case ConnectionStep::TIMEOUT:
            scheduleStep(reconnectInterval, ConnectionStep::INFO_REQUEST);
            break;
        case ConnectionStep::INFO_REQUEST: infoRequest(); break;
        case ConnectionStep::WEBSOCKET: openWS(); break;
        default: Q_ASSERT_X(false, "makeStep", "Unexpected connection step"); break;
    }
}

} // namespace mpk::rest
