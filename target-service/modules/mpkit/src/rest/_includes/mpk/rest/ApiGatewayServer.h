#pragma once

#include "mpk/rest/ApiGatewaySettings.h"
#include "mpk/rest/Server.h"

#include <filesystem>
#include <memory>

#include <QObject>

namespace http
{

struct ConnectionSettings;
class HtpasswdAuthStorage;
class HttpServer;

} // namespace http

namespace mpk::rest
{

class AuthTokenStorage;
class ApiDescription;
class UserPermissionsProvider;

} // namespace mpk::rest

namespace mpk::rest
{

class ApiGatewayServer final : public Server
{
public:
    explicit ApiGatewayServer(ApiGatewaySettings settings);
    ApiGatewayServer(ApiGatewayServer&&) noexcept;
    ~ApiGatewayServer() override;

private:
    void run() override;
    ApiGatewaySingle apiGateway() const override;
    AuthorizeMethodSingle authorizeMethod() const override;

    using HttpServer = http::HttpServer;
    using HttpServerPtr = std::unique_ptr<HttpServer>;
    using ConnectionSettings = http::ConnectionSettings;

    using ApiDescriptionPtr = std::unique_ptr<rest::ApiDescription>;

    void initHtpasswdAuthStorage(const std::filesystem::path& passwdFile);
    void initUserPermissionsProvider(const std::optional<std::filesystem::path>& configFile);
    void initAuthTokenStorage(const std::filesystem::path& authFile);

    HttpServerPtr createHttpServer(const ConnectionSettings& settings);

    void initApiGateway(HttpServerPtr httpServer, ApiDescriptionPtr apiDescription);

private:
    using AuthTokenStoragePtr = std::unique_ptr<rest::AuthTokenStorage>;

    using HtpasswdAuthStorage = http::HtpasswdAuthStorage;
    using HtpasswdAuthStoragePtr = std::unique_ptr<HtpasswdAuthStorage>;

    using UserPermissionsProviderUnique = std::unique_ptr<UserPermissionsProvider>;

    ApiGatewaySettings m_settings;

    AuthTokenStoragePtr m_tokenStorage;
    HtpasswdAuthStoragePtr m_htpasswdAuthStorage;
    UserPermissionsProviderUnique m_userPermissionsProvider;
    std::unique_ptr<rest::ApiGateway> m_apiGateway;

    bool m_isRunning{false};
};

} // namespace mpk::rest
