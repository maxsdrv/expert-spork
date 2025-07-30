#include "mpk/rest/HttpWsClient.h"
#include "mpk/rest/HttpUtility.h"

#include "httpws/AuthHttpClient.h"
#include "httpws/Call.h"
#include "httpws/HttpClient.h"
#include "httpws/ws/WebSocketClient.h"

#include "gsl/assert"

#include <QThread>

namespace mpk::rest
{

HttpWsClient::HttpWsClient(gsl::not_null<const HttpWsConnection*> connection) :
  m_connection{connection}, m_signals{std::make_unique<LinkSignals>()}
{
    connectHttpWsConnectionSignals();
}

void HttpWsClient::get(const QString& operation, JsonResultHandler resultHandler) const
{
    get(operation, {}, std::move(resultHandler));
}

void HttpWsClient::get(
    const QString& operation,
    const QJsonObject& parameters,
    JsonResultHandler resultHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::GET,
        operation,
        parameters,
        std::move(resultHandler),
        ErrorCodeHandler{});
}

void HttpWsClient::get(
    const QString& operation,
    const QJsonObject& parameters,
    JsonResultHandler resultHandler,
    ErrorCodeHandler errorHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::GET,
        operation,
        parameters,
        std::move(resultHandler),
        std::move(errorHandler));
}

void HttpWsClient::get(
    const QString& operation,
    const QJsonObject& parameters,
    TextResultHandler resultHandler,
    ErrorCodeHandler errorHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::GET,
        operation,
        parameters,
        std::move(resultHandler),
        std::move(errorHandler));
}

void HttpWsClient::post(const QString& operation, const QJsonObject& parameters) const
{
    post(operation, parameters, JsonResultHandler{}, ErrorCodeHandler{});
}

void HttpWsClient::post(
    const QString& operation, const QJsonObject& parameters, ErrorCodeHandler errorHandler) const
{
    post(operation, parameters, JsonResultHandler{}, std::move(errorHandler));
}

void HttpWsClient::post(
    const QString& operation,
    const QJsonObject& parameters,
    JsonResultHandler resultHandler,
    ErrorCodeHandler errorHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::POST,
        operation,
        parameters,
        std::move(resultHandler),
        std::move(errorHandler));
}

void HttpWsClient::put(const QString& operation, const QJsonObject& parameters) const
{
    put(operation, parameters, JsonResultHandler{}, ErrorCodeHandler{});
}

void HttpWsClient::put(
    const QString& operation,
    const QJsonObject& parameters,
    JsonResultHandler resultHandler) const
{
    put(operation, parameters, std::move(resultHandler), ErrorCodeHandler{});
}

void HttpWsClient::put(
    const QString& operation,
    const QJsonObject& parameters,
    JsonResultHandler resultHandler,
    ErrorCodeHandler errorHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::PUT,
        operation,
        parameters,
        std::move(resultHandler),
        std::move(errorHandler));
}

void HttpWsClient::del(
    const QString& operation, const QJsonObject& parameters, ErrorCodeHandler errorHandler) const
{
    arbitraryThreadRequest(
        HttpMethod::Value::DELETE,
        operation,
        parameters,
        JsonResultHandler{},
        std::move(errorHandler));
}

template <class ResultHandler>
void HttpWsClient::arbitraryThreadRequest(
    HttpMethod::Value method,
    const QString& operation,
    const QJsonObject& parameters,
    ResultHandler resultHandler,
    ErrorCodeHandler errorHandler) const
{
    if (inHttpThread())
    {
        httpThreadRequest(
            method, operation, parameters, std::move(resultHandler), std::move(errorHandler));
    }
    else
    {
#ifndef __clang_analyzer__
        QMetaObject::invokeMethod(
            &httpClient(),
            [this,
             method,
             operation,
             parameters,
             resultHandler = std::move(resultHandler),
             errorHandler = std::move(errorHandler)]
            {
                httpThreadRequest(
                    method,
                    operation,
                    parameters,
                    std::move(resultHandler),
                    std::move(errorHandler));
            },
            Qt::QueuedConnection);
#endif
        return;
    }
}

template <class ResponseResult>
void HttpWsClient::httpThreadRequest(
    HttpMethod::Value method,
    const QString& operation,
    const QJsonObject& parameters,
    std::function<void(const ResponseResult& result)> resultHandler,
    ErrorCodeHandler errorHandler) const
{
    if (!isConnected())
    {
        handleDisconnect(operation, errorHandler);
        return;
    }

    httpClient()
        .httpRequest(method, operation, parameters)
        .onSuccess(
            [this, operation, resultHandler = std::move(resultHandler)](
                const ResponseResult& result)
            {
                if (resultHandler)
                {
                    resultHandler(result);
                }

                m_connection->reportOperationResult(operation, OperationResult::SUCCESS);
            })
        .onError(
            [this,
             operation,
             errorHandler = std::move(errorHandler)](int code, const QString& /*text*/)
            {
                if (errorHandler)
                {
                    errorHandler(code);
                }

                m_connection->reportOperationResult(
                    operation, toOperationResult(static_cast<http::HttpCode>(code)));
            })
        .run();
}

void HttpWsClient::subscribe(const QString& messageType, MessageHandler messageHandler) const
{
    webSocketClient().registerHandler(messageType, std::move(messageHandler));
}

void HttpWsClient::unsubscribe(const QString& messageType) const
{
    webSocketClient().unregisterHandler(messageType);
}

http::HttpClient& HttpWsClient::httpClient() const
{
    return m_connection->httpClient();
}

http::WebSocketClient& HttpWsClient::webSocketClient() const
{
    return m_connection->webSocketClient();
}

void HttpWsClient::connectHttpWsConnectionSignals() const
{
    QObject::connect(
        m_connection,
        &HttpWsConnection::stateChanged,
        m_signals.get(),
        [&] { emit m_signals->linkStateChanged(linkState()); });
}

bool HttpWsClient::isConnected() const
{
    return m_connection->state() == ConnectionState::CONNECTED;
}

void HttpWsClient::handleDisconnect(
    const QString& operation, const ErrorCodeHandler& errorHandler) const
{
    if (errorHandler)
    {
        const int code = 2 /*httplib::Error::Connection*/;
        errorHandler(code);
    }

    m_connection->reportOperationResult(
        operation, toOperationResult(http::HttpCode::Unauthorized));
}

bool HttpWsClient::inHttpThread() const
{
    return QThread::currentThread() == httpClient().thread();
}

HttpWsClient::LinkState HttpWsClient::linkState() const
{
    switch (m_connection->state())
    {
        case ConnectionState::DISCONNECTED: return LinkState::DISCONNECTED;
        case ConnectionState::CONNECTING: return LinkState::CONNECTING;
        case ConnectionState::CONNECTED: return LinkState::CONNECTED;

        default: Ensures(false);
    }
}

HttpWsClient::LinkSignalsPin HttpWsClient::linkSignals() const
{
    return m_signals.get();
}

} // namespace mpk::rest
