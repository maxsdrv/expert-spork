#include "ApiRestServer.h"

#include "mpk/rest/ApiDescriptionJsonFile.h"
#include "mpk/rest/ApiGateway.h"

#include "httpws/HttpServer.h"

#include "qt/Strings.h"

namespace mpk::dss
{

ApiRestServer::ApiRestServer(
    http::ConnectionSettings connectionSettings,
    std::string apiDescriptionFile) :
  m_connectionSettings(std::move(connectionSettings)),
  m_apiDescriptionFile(std::move(apiDescriptionFile))
{
}

ApiRestServer::~ApiRestServer() = default;

void ApiRestServer::run()
{
    if (m_isRunning)
    {
        return;
    }

    auto httpServer = createHttpServer(m_connectionSettings);
    auto apiDescription = createApiDescription(m_apiDescriptionFile);

    initApiGateway(std::move(httpServer), std::move(apiDescription));

    m_isRunning = true;
}

auto ApiRestServer::apiGateway() const -> ApiGatewaySingle
{
    return m_apiGateway.get();
}

ApiRestServer::HttpServerPtr ApiRestServer::createHttpServer(
    const ConnectionSettings& settings)
{
    auto httpServer = std::make_unique<http::HttpServer>(settings);
    httpServer->listen();
    return httpServer;
}

ApiRestServer::ApiDescriptionPtr ApiRestServer::createApiDescription(
    const std::string& apiFile)
{
    using mpk::rest::ApiDescriptionJsonFile;
    return std::make_unique<ApiDescriptionJsonFile>(apiFile);
}

void ApiRestServer::initApiGateway(
    HttpServerPtr httpServer, ApiDescriptionPtr apiDescription)
{
    m_apiGateway = std::make_unique<rest::ApiGateway>(
        std::move(httpServer), std::move(apiDescription));
}

} // namespace mpk::dss
