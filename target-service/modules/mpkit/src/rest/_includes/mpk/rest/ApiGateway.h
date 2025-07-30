#pragma once

#include "httpws/HttpMethod.h"

#include <QHash>
#include <QObject>

#include <functional>
#include <memory>
#include <variant>

namespace http
{

class HttpServer;
struct HttpRoute;

} // namespace http

namespace mpk::rest
{

class ApiDescription;
class ApiGateway : public QObject
{
    Q_OBJECT

public:
    ApiGateway(
        std::unique_ptr<http::HttpServer> server,
        std::unique_ptr<ApiDescription> apiDescription,
        QObject* parent = nullptr);
    ~ApiGateway() override;

    // Sync handlers will be called in SEPARATE THREAD and should
    // return QJsonObject with 'answer'
    // Using Http::GET method
    // Typical usage: getSomething
    using JsonParams = QJsonObject;
    using JsonResult = QJsonObject;
    using SyncJsonToJsonHandler = std::function<JsonResult(const JsonParams&)>;

    using StringResult = std::string;
    using SyncJsonToStringHandler = std::function<StringResult(const JsonParams&)>;

    using SyncNoneToJsonHandler = std::function<JsonResult(void)>;

    void registerSyncHandler(const QString& name, const SyncJsonToJsonHandler& handler);
    void registerSyncHandler(
        const QString& name,
        http::HttpMethod::Value method,
        const SyncJsonToJsonHandler& handler);

    void registerSyncHandler(const QString& name, const SyncJsonToStringHandler& handler);

    void registerSyncHandler(const QString& name, const SyncNoneToJsonHandler& handler);
    void registerSyncHandler(
        const QString& name,
        http::HttpMethod::Value method,
        const SyncNoneToJsonHandler& handler);

    // Async handlers will be called in main thread after Qt message queue
    // roll. Empty QJsonObject will be returned to caller
    // Using Http::POST method
    // Typical usage: setSomething
    using AsyncJsonToNoneHandler = std::function<void(const JsonParams&)>;
    void registerAsyncHandler(const QString& name, const AsyncJsonToNoneHandler& handler);
    void registerAsyncHandler(
        const QString& name,
        http::HttpMethod::Value method,
        const AsyncJsonToNoneHandler& handler);
    // NoArgsAsyncHandler seems useless

private:
    void runAsyncHandler(const QString& name, const JsonParams& params);
    static void runAsyncHandler(
        const QString& name, const JsonParams& params, const AsyncJsonToNoneHandler& handler);
    void runAsyncHandlerWithMethod(
        const QString& name, http::HttpMethod::Value method, const JsonParams& params);

    template <class HandlerType>
    void addRoute(const http::HttpRoute& route, const HandlerType& handler);

private:
    using PathWithMethod = std::pair<QString, http::HttpMethod::Value>;

    std::unique_ptr<http::HttpServer> m_server;
    std::unique_ptr<ApiDescription> m_apiDescription;
    std::map<QString, AsyncJsonToNoneHandler> m_asyncHandlers;

    QHash<PathWithMethod, AsyncJsonToNoneHandler> m_asyncHandlersWithMethod;
};

} // namespace mpk::rest
