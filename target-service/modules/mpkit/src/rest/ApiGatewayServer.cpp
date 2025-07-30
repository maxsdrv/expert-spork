#include "mpk/rest/ApiGatewayServer.h"

#include "mpk/rest/ApiGatewaySettings.h"

#include "mpk/rest/ApiDescriptionJsonFile.h"
#include "mpk/rest/ApiGateway.h"
#include "mpk/rest/AuthTokenStorage.h"
#include "mpk/rest/UserPermissionsJsonFile.h"
#include "mpk/rest/UserPermissionsStatic.h"

#include "httpws/AuthHttpServer.h"
#include "httpws/HtpasswdAuthStorage.h"

#include "qt/Strings.h"

namespace mpk::rest
{

ApiGatewayServer::ApiGatewayServer(ApiGatewaySettings settings) : m_settings{std::move(settings)}
{
}

ApiGatewayServer::ApiGatewayServer(ApiGatewayServer&&) noexcept = default;
ApiGatewayServer::~ApiGatewayServer() = default;

void ApiGatewayServer::run()
{
    if (m_isRunning)
    {
        return;
    }

    initHtpasswdAuthStorage(m_settings.passwdFile);
    initUserPermissionsProvider(m_settings.userPermissionsFile);
    initAuthTokenStorage(m_settings.authFile);

    auto httpServer = createHttpServer(m_settings.connectionSettings);
    auto apiDescription =
        std::make_unique<mpk::rest::ApiDescriptionJsonFile>(m_settings.apiDescriptionFile);

    initApiGateway(std::move(httpServer), std::move(apiDescription));

    m_isRunning = true;
}

ApiGatewayServer::ApiGatewaySingle ApiGatewayServer::apiGateway() const
{
    return m_apiGateway.get();
}

ApiGatewayServer::AuthorizeMethodSingle ApiGatewayServer::authorizeMethod() const
{
    return m_tokenStorage.get();
}

void ApiGatewayServer::initHtpasswdAuthStorage(const std::filesystem::path& passwdFile)
{
    m_htpasswdAuthStorage = std::make_unique<HtpasswdAuthStorage>(passwdFile);
}

void ApiGatewayServer::initUserPermissionsProvider(
    const std::optional<std::filesystem::path>& configFile)
{
    // TODO: Remove the inline permissions once all relevant projects have the config file.

    if (configFile)
    {
        m_userPermissionsProvider = std::make_unique<UserPermissionsJsonFile>(*configFile);
    }
    else
    {
        UserPermissionsStatic::GroupsMap groups{
            {"ADMIN", {"admin"}},
            {"OPERATOR", {"operator"}},
            {"MAINTAINER", {"maintainer"}},
        };
        m_userPermissionsProvider = std::make_unique<UserPermissionsStatic>(std::move(groups));
    }
}

void ApiGatewayServer::initAuthTokenStorage(const std::filesystem::path& authFile)
{
    m_tokenStorage = std::make_unique<mpk::rest::AuthTokenStorage>(
        m_htpasswdAuthStorage.get(), m_userPermissionsProvider.get(), authFile);
}

ApiGatewayServer::HttpServerPtr ApiGatewayServer::createHttpServer(
    const ConnectionSettings& settings)
{
    auto httpServer = std::make_unique<http::AuthHttpServer>(
        settings, m_tokenStorage.get(), m_tokenStorage.get());
    httpServer->listen();
    return httpServer;
}

void ApiGatewayServer::initApiGateway(HttpServerPtr httpServer, ApiDescriptionPtr apiDescription)
{
    m_apiGateway =
        std::make_unique<rest::ApiGateway>(std::move(httpServer), std::move(apiDescription));
}

} // namespace mpk::rest
