#pragma once

#include "ConnectionSettings.h"
#include "HttpRoute.h"

#include <QObject>

#include <functional>
#include <memory>
#include <set>
#include <thread>

namespace httplib
{

struct Request;
struct Response;

class Server;

} // namespace httplib

namespace http
{

class AuthorizeMethod;

class HttpServer : public QObject
{
    Q_OBJECT

public:
    using JsonParams = QJsonObject;
    using JsonResult = QJsonObject;
    using JsonToJsonHandler = std::function<JsonResult(const JsonParams&)>;

    using StringResult = std::string;
    using JsonToStringHandler = std::function<StringResult(const JsonParams&)>;

    using NoneToJsonHandler = std::function<JsonResult(void)>;

    explicit HttpServer(
        ConnectionSettings settings,
        AuthorizeMethod* authorizeMethod = nullptr,
        QObject* parent = nullptr);
    ~HttpServer() override;

    void listen();

    void addRoute(const HttpRoute& route, const JsonToJsonHandler& handler);
    void addRoute(const HttpRoute& route, const JsonToStringHandler& handler);
    void addRoute(const HttpRoute& route, const NoneToJsonHandler& handler);

    // TODO Remove this methods
    void addRoute(
        http::HttpMethod::Value value, const QString& path, const JsonToJsonHandler& handler)
    {
        addRoute(HttpRoute{value, path, {}}, handler);
    }
    void addRoute(
        http::HttpMethod::Value value, const QString& path, const NoneToJsonHandler& handler)
    {
        addRoute(HttpRoute{value, path, {}}, handler);
    }

    const std::set<HttpRoute>& routes() const;

    void addMountPoint(const std::string& publicPath, const std::string& serverPath);

protected:
    using Request = httplib::Request;
    using Response = httplib::Response;

    QString registerRoute(const HttpRoute& route);

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::unique_ptr<httplib::Server> m_server;

private:
    using httplibHandler = std::function<void(const Request&, Response&)>;

    void authorize(const Request& request, const Permissions& routePermissions);

    template <class HandlerType>
    void addRouteWithHandler(const HttpRoute& route, const HandlerType& handler);

    template <class HandlerType>
    httplibHandler createHttpHandler(const HttpRoute& route, const HandlerType& handler);

    void addHandler(const HttpRoute& route, const httplibHandler& handler);

private:
    ConnectionSettings m_settings;
    AuthorizeMethod* m_authorizeMethod;
    std::thread m_thread;

    std::set<HttpRoute> m_registeredRoutes;

signals:
    void listenFailure(int error_code = -1);
};

std::string authHeaderValue(const httplib::Request& request);

} // namespace http
