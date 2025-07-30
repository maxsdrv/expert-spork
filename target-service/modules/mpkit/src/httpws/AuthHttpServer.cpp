#include "httpws/AuthHttpServer.h"

#include "httpws/AuthUtils.h"
#include "httpws/AuthenticateMethod.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"

#include "json/JsonUtilities.h"

#include "log/Log2.h"

#include "httplib.h"

namespace http
{

namespace
{

void loginHandler(
    const httplib::Request& request,
    httplib::Response& response,
    gsl::not_null<AuthenticateMethod*> authenticateMethod)
{
    using namespace exception::http;
    LOG_INFO << "Authentication request from " << request.remote_addr << ":"
             << request.remote_port;
    try
    {
        auto authParams = parseAuthHeaderParams(authHeaderValue(request));

        AuthTokens tokens;
        switch (authParams.type)
        {
            case AuthType::BASIC:
                tokens = authenticateMethod->getTokens(parseBasicAuthData(authParams.data));
                break;
            case AuthType::BEARER: tokens = authenticateMethod->getTokens(authParams.data); break;
        }

        auto tokensJson =
            json::toStdString(json::toValue(tokens).toObject(), QJsonDocument::Compact);
        response.set_content(tokensJson, jsonContentType);
        response.status = static_cast<int>(HttpCode::Ok);
    }
    catch (std::exception& ex)
    {
        response.status = static_cast<int>(responseErrorStatus(ex));
        auto errorMessage = responseErrorMessage(ex);
        response.set_content(errorMessage, textContentType);
        LOG_WARNING << "Auth exception with code " << response.status << ": " << errorMessage;
    }
}

} // namespace

AuthHttpServer::AuthHttpServer(
    const ConnectionSettings& settings,
    gsl::not_null<AuthenticateMethod*> authenticateMethod,
    gsl::not_null<AuthorizeMethod*> authorizeMethod,
    QObject* parent) :
  HttpServer(settings, authorizeMethod, parent)
{
    const auto resolvedPath =
        registerRoute(
            HttpRoute{
                HttpMethod::POST, loginPath, {Permissions::Groups{Permissions::UNAUTHORIZED}}})
            .toStdString();
    m_server->Post(
        resolvedPath,
        [this, authenticateMethod](const auto& request, auto& response)
        { loginHandler(request, response, authenticateMethod); });
}

} // namespace http
