#pragma once

#include "httpws/ConnectionSettings.h"

#include <QObject>

#include "gsl/pointers"

#include <memory>

namespace http
{

class HttpServer;

} // namespace http

namespace mpk::rest
{

class ApiGateway;
class ApiDescription;

} // namespace mpk::rest

namespace mpk::dss::core
{

class ApiRestServer
{
public:
    ApiRestServer(
        http::ConnectionSettings connectionSettings,
        std::string apiDescriptionFile);
    ~ApiRestServer();

    using ApiGatewaySingle = gsl::not_null<rest::ApiGateway*>;

    void run();
    [[nodiscard]] ApiGatewaySingle apiGateway() const;

private:
    using HttpServer = http::HttpServer;
    using HttpServerPtr = std::unique_ptr<HttpServer>;
    using ConnectionSettings = http::ConnectionSettings;

    using ApiDescriptionPtr = std::unique_ptr<rest::ApiDescription>;

    static HttpServerPtr createHttpServer(const ConnectionSettings& settings);
    static ApiDescriptionPtr createApiDescription(const std::string& apiFile);

    void initApiGateway(
        HttpServerPtr httpServer, ApiDescriptionPtr apiDescription);

private:
    http::ConnectionSettings m_connectionSettings;
    std::string m_apiDescriptionFile;

    std::unique_ptr<rest::ApiGateway> m_apiGateway;

    bool m_isRunning{false};
};

} // namespace mpk::dss::core
