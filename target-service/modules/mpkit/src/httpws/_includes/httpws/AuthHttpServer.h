#pragma once

#include "httpws/HttpServer.h"

#include "gsl/pointers"

namespace http
{

class AuthenticateMethod;
class AuthorizeMethod;

class AuthHttpServer : public HttpServer
{
    Q_OBJECT

public:
    AuthHttpServer(
        const ConnectionSettings& settings,
        gsl::not_null<AuthenticateMethod*> authenticateMethod,
        gsl::not_null<AuthorizeMethod*> authorizeMethod,
        QObject* parent = nullptr);
};

} // namespace http
