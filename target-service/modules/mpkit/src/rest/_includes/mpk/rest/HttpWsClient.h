#pragma once

#include "httpws/HttpMethod.h"

#include "mpk/rest/Client.h"
#include "mpk/rest/HttpWsConnection.h"

#include "gsl/pointers"

#include <memory>

namespace mpk::rest
{

class HttpWsClient final : public Client
{
public:
    explicit HttpWsClient(gsl::not_null<const HttpWsConnection*> connection);

private:
    void get(const QString& operation, JsonResultHandler resultHandler) const override;
    void get(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler) const override;
    void get(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const override;
    void get(
        const QString& operation,
        const QJsonObject& parameters,
        TextResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const override;

    void post(const QString& operation, const QJsonObject& parameters) const override;
    void post(
        const QString& operation,
        const QJsonObject& parameters,
        ErrorCodeHandler errorHandler) const override;
    void post(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const override;

    void put(const QString& operation, const QJsonObject& parameters) const override;
    void put(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler) const override;
    void put(
        const QString& operation,
        const QJsonObject& parameters,
        JsonResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const override;

    void del(
        const QString& operation,
        const QJsonObject& parameters,
        ErrorCodeHandler errorHandler) const override;

    using HttpMethod = http::HttpMethod;

    template <class ResultHandler>
    void arbitraryThreadRequest(
        HttpMethod::Value method,
        const QString& operation,
        const QJsonObject& parameters,
        ResultHandler resultHandler,
        ErrorCodeHandler errorHandler) const;

    template <class ResponseResult>
    void httpThreadRequest(
        HttpMethod::Value method,
        const QString& operation,
        const QJsonObject& parameters,
        std::function<void(const ResponseResult& result)> resultHandler,
        ErrorCodeHandler errorHandler) const;

    void subscribe(const QString& messageType, MessageHandler messageHandler) const override;
    void unsubscribe(const QString& messageType) const override;

private:
    LinkState linkState() const override;

    LinkSignalsPin linkSignals() const override;

private:
    http::HttpClient& httpClient() const;
    http::WebSocketClient& webSocketClient() const;

    void connectHttpWsConnectionSignals() const;

    bool isConnected() const;
    void handleDisconnect(const QString& operation, const ErrorCodeHandler& errorHandler) const;

    bool inHttpThread() const;

private:
    gsl::not_null<const HttpWsConnection*> m_connection;
    std::unique_ptr<LinkSignals> m_signals;
};

} // namespace mpk::rest
