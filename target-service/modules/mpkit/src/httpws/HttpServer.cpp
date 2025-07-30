#include "httpws/HttpServer.h"

#include "httpws/AuthUtils.h"
#include "httpws/AuthorizeMethod.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"

#include "exception/DuplicateEntity.h"
#include "exception/KeyInfo.h"

#include "json/JsonUtilities.h"

#include "log/Log2.h"

#include "qt/Strings.h"

#include "httplib.h"

#undef DELETE

using namespace exception::http;

namespace http
{

namespace
{

constexpr auto AUTHORIZATION_HEADER = "Authorization";

void checkRequestType(const httplib::Request& request, const std::string& contentType)
{
    if (const auto headerValue = request.get_header_value(contentTypeHeader);
        headerValue != contentType)
    {
        auto message = std::string("Content type '") + headerValue + "' isn't equal to '"
                       + contentType + "'";
        BOOST_THROW_EXCEPTION(
            Exception()
            << ResponseStatus(HttpCode::UnsupportedMediaType) << ResponseMessage(message));
    }
}

void setContent(httplib::Response& response, const QJsonObject& json)
{
    response.set_content(
        json::toByteArray(json, QJsonDocument::Compact).toStdString(), jsonContentType);
}

void setContent(httplib::Response& response, const std::string& text)
{
    response.set_content(text, textContentType);
}

void addAuthorization(const httplib::Request& request, QJsonObject& json)
{
    // TODO: temporary implementation until handlers don't analyze headers
    if (!request.has_header(AUTHORIZATION_HEADER))
    {
        return;
    }

    if (auto authParams = parseAuthHeaderParams(authHeaderValue(request));
        authParams.type == AuthType::BEARER)
    {
        json.insert("token", authParams.data);
    }
}

QJsonObject convertRequestBodyToJson(const httplib::Request& request)
{
    checkRequestType(request, jsonContentType);

    if (!request.body.empty())
    {
        auto result = json::fromByteArray(QByteArray::fromStdString(request.body));
        addAuthorization(request, result);
        return result;
    }

    return {};
}

QJsonObject convertRequestParamsToJson(const httplib::Request& request)
{
    if (!request.params.empty())
    {
        auto result = json::toValue(request.params).toObject();
        addAuthorization(request, result);
        return result;
    }

    return {};
}

} // namespace

bool operator<(const HttpRoute& lhs, const HttpRoute& rhs)
{
    return std::tie(lhs.path, lhs.method) < std::tie(rhs.path, rhs.method);
}

HttpServer::HttpServer(
    ConnectionSettings settings, AuthorizeMethod* authorizeMethod, QObject* parent) :
  QObject(parent),
  m_server(std::make_unique<httplib::Server>()),
  m_settings{std::move(settings)},
  m_authorizeMethod(authorizeMethod)
{
}

HttpServer::~HttpServer()
{
    if (m_server->is_running())
    {
        m_server->stop();
    }
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void HttpServer::addRoute(const HttpRoute& route, const JsonToJsonHandler& handler)
{
    addRouteWithHandler(route, handler);
}

void HttpServer::addRoute(const HttpRoute& route, const JsonToStringHandler& handler)
{
    addRouteWithHandler(route, handler);
}

void HttpServer::addRoute(const HttpRoute& route, const NoneToJsonHandler& handler)
{
    addRouteWithHandler(route, handler);
}

template <class HandlerType>
void HttpServer::addRouteWithHandler(const HttpRoute& route, const HandlerType& handler)
{
    Q_ASSERT(handler);

    auto httpHandler = createHttpHandler(route, handler);
    addHandler(route, httpHandler);
}

template <class HandlerType>
HttpServer::httplibHandler HttpServer::createHttpHandler(
    const HttpRoute& route, const HandlerType& handler)
{
    auto httpHandler = [this,
                        method = route.method,
                        handler = std::move(handler),
                        permissions = route.permissions](const auto& request, auto& response)
    {
        try
        {
            LOG_DEBUG << "Handle " << request.path << " path";
            authorize(request, permissions);

            // clang-format off
            bool constexpr handlerHasJsonParams =
                   std::is_same_v<JsonToJsonHandler, HandlerType>
                || std::is_same_v<JsonToStringHandler, HandlerType>;
            // clang-format on
            if constexpr (handlerHasJsonParams)
            {
                auto object = method == HttpMethod::GET ? convertRequestParamsToJson(request) :
                                                          convertRequestBodyToJson(request);
                auto result = handler(object);
                setContent(response, result);
            }
            else
            {
                // avoid 'lambda capture 'method' is not used' error
                Q_UNUSED(method);
                auto result = handler();
                setContent(response, result);
            }
        }
        catch (const exception::http::Exception& ex)
        {
            response.status = static_cast<int>(*boost::get_error_info<ResponseStatus>(ex));
            response.set_content(
                static_cast<std::string>(*boost::get_error_info<ResponseMessage>(ex)),
                textContentType);

            LOG_ERROR << "HttpServer http exception: " << ex.what();
            return;
        }
        catch (const exception::json::InvalidJson& ex)
        {
            response.status = static_cast<int>(HttpCode::BadRequest);
            response.set_content(ex.what(), textContentType);

            LOG_ERROR << "HttpServer invalid json: " << ex.what();
        }
        catch (const std::exception& ex)
        {
            response.status = static_cast<int>(HttpCode::InternalServerError);
            response.set_content(ex.what(), textContentType);

            LOG_ERROR << "HttpServer: " << ex.what();
        }
    };

    return httpHandler;
}

void HttpServer::addHandler(const HttpRoute& route, const httplibHandler& handler)
{
    auto resolvedPath = registerRoute(route).toStdString();
    switch (route.method)
    {
        case HttpMethod::DELETE: m_server->Delete(resolvedPath, handler); break;
        case HttpMethod::GET: m_server->Get(resolvedPath, handler); break;
        case HttpMethod::POST: m_server->Post(resolvedPath, handler); break;
        case HttpMethod::PUT: m_server->Put(resolvedPath, handler); break;
        default: BOOST_THROW_EXCEPTION(exception::http::UnsupportedMethod(route.method));
    }
}

void HttpServer::listen()
{
    auto host = m_settings.host.toStdString();
    auto port = m_settings.port;

    m_thread = std::thread(
        [this, host, port]()
        {
            if (!m_server->listen(host, port))
            {
                LOG_FATAL << "Can't listen host: " << host << " port: " << port;
                emit listenFailure();
            }
        });
    LOG_INFO << "Http server is listening on " << host << ":" << port;
}

const std::set<HttpRoute>& HttpServer::routes() const
{
    return m_registeredRoutes;
}

void HttpServer::addMountPoint(const std::string& publicPath, const std::string& serverPath)
{
    const bool result = m_server->set_mount_point(publicPath, serverPath);
    if (result)
    {
        LOG_DEBUG << "Registered mount point '" << publicPath << "' for folder '" << serverPath
                  << "'";
    }
    else
    {
        LOG_ERROR << "Can't mount point '" << publicPath << "' for folder '" << serverPath << "'";
    }
}

QString HttpServer::registerRoute(const HttpRoute& route)
{
    if (m_registeredRoutes.find(route) != m_registeredRoutes.end())
    {
        BOOST_THROW_EXCEPTION(
            exception::DuplicateEntity() << exception::KeyInfo(strings::toUtf8(route.path)));
    }
    m_registeredRoutes.insert(route);

    auto path = resolvePath(API_VERSION, route.path);

    LOG_DEBUG << "Registered route " << http::HttpMethod::toString(route.method) << " " << path;

    return path;
}

void HttpServer::authorize(const httplib::Request& request, const Permissions& routePermissions)
{
    if (m_authorizeMethod != nullptr && !routePermissions.unauthorized())
    {
        auto authParams = parseAuthHeaderParams(authHeaderValue(request));
        if (authParams.type != AuthType::BEARER)
        {
            BOOST_THROW_EXCEPTION(
                Exception() << ResponseStatus(HttpCode::NotAcceptable)
                            << ResponseMessage("Auth type not supported"));
        }

        auto userPermissions = m_authorizeMethod->validate(authParams.data);
        if (userPermissions.empty())
        {
            BOOST_THROW_EXCEPTION(
                Exception() << ResponseStatus(HttpCode::Unauthorized)
                            << ResponseMessage("Token is not valid"));
        }

        if (!routePermissions.match(userPermissions))
        {
            BOOST_THROW_EXCEPTION(
                Exception() << ResponseStatus(HttpCode::Forbidden)
                            << ResponseMessage("Operation not permitted"));
        }
    }
}

std::string authHeaderValue(const httplib::Request& request)
{
    if (!request.has_header(AUTHORIZATION_HEADER))
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(http::HttpCode::Unauthorized)
                        << ResponseMessage("Authorization needed"));
    }

    return request.get_header_value(AUTHORIZATION_HEADER);
}

} // namespace http
