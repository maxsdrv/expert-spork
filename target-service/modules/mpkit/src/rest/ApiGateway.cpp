#include "mpk/rest/ApiGateway.h"

#include "mpk/rest/ApiDescription.h"

#include "httpws/Definitions.h"
#include "httpws/HttpServer.h"

#include "exception/DuplicateEntity.h"
#include "exception/MissingEntity.h"

#include "log/Log2.h"

#include "qt/Strings.h"

#include "json/JsonUtilities.h"

namespace mpk::rest
{

ApiGateway::ApiGateway(
    std::unique_ptr<http::HttpServer> server,
    std::unique_ptr<ApiDescription> apiDescription,
    QObject* parent) :
  QObject(parent), m_server(std::move(server)), m_apiDescription(std::move(apiDescription))
{
}

ApiGateway::~ApiGateway() = default;

void ApiGateway::registerSyncHandler(const QString& name, const SyncJsonToJsonHandler& handler)
{
    auto route = m_apiDescription->route(name);
    addRoute(route, handler);
}

void ApiGateway::registerSyncHandler(
    const QString& name, http::HttpMethod::Value method, const SyncJsonToJsonHandler& handler)
{
    auto route = m_apiDescription->route(name, method);
    addRoute(route, handler);
}

void ApiGateway::registerSyncHandler(const QString& name, const SyncJsonToStringHandler& handler)
{
    auto route = m_apiDescription->route(name);
    addRoute(route, handler);
}

void ApiGateway::registerSyncHandler(const QString& name, const SyncNoneToJsonHandler& handler)
{
    auto route = m_apiDescription->route(name);
    addRoute(route, handler);
}

void ApiGateway::registerSyncHandler(
    const QString& name, http::HttpMethod::Value method, const SyncNoneToJsonHandler& handler)
{
    auto route = m_apiDescription->route(name, method);
    addRoute(route, handler);
}

void ApiGateway::registerAsyncHandler(const QString& name, const AsyncJsonToNoneHandler& handler)
{
    auto route = m_apiDescription->route(name);
    m_server->addRoute(
        route,
        [this, name](const JsonParams& params) -> JsonResult
        {
            // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
            QMetaObject::invokeMethod(
                this,
                [this, name, params] { runAsyncHandler(name, params); },
                Qt::QueuedConnection);

            return {};
        });
    m_asyncHandlers[name] = handler;
}

void ApiGateway::registerAsyncHandler(
    const QString& name, http::HttpMethod::Value method, const AsyncJsonToNoneHandler& handler)
{
    auto route = m_apiDescription->route(name, method);
    m_server->addRoute(
        route,
        [this, name, method](const JsonParams& params) -> JsonResult
        {
            // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
            QMetaObject::invokeMethod(
                this,
                [this, name, method, params] { runAsyncHandlerWithMethod(name, method, params); },
                Qt::QueuedConnection);

            return {};
        });
    m_asyncHandlersWithMethod[{name, method}] = handler;
}

void ApiGateway::runAsyncHandler(const QString& name, const JsonParams& params)
{
    auto it = m_asyncHandlers.find(name);
    if (it != m_asyncHandlers.end())
    {
        runAsyncHandler(name, params, it->second);
    }
    else
    {
        LOG_ERROR << "Async handler not found: " << strings::toUtf8(name);
    }
}

void ApiGateway::runAsyncHandlerWithMethod(
    const QString& name, http::HttpMethod::Value method, const JsonParams& params)
{
    auto it = m_asyncHandlersWithMethod.find({name, method});
    if (it != m_asyncHandlersWithMethod.end())
    {
        runAsyncHandler(name, params, it.value());
    }
    else
    {
        LOG_ERROR << "Async handler not found: " << strings::toUtf8(name)
                  << ", for method: " << http::HttpMethod::toString(method);
    }
}

void ApiGateway::runAsyncHandler(
    const QString& name, const JsonParams& params, const AsyncJsonToNoneHandler& handler)
{
    try
    {
        LOG_DEBUG << "ApiGateway path: " << name;
        LOG_TRACE << "ApiGateway json: " << json::toByteArray(params).toStdString();

        handler(params);
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "Async handler run failed: " << strings::toUtf8(name) << " - " << ex.what();
    }
}

template <class HandlerType>
void ApiGateway::addRoute(const http::HttpRoute& route, const HandlerType& handler)
{
    m_server->addRoute(route, handler);
}

} // namespace mpk::rest
