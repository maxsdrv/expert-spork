#pragma once

#include "gsl/pointers"

namespace http
{

class AuthorizeMethod;

} // namespace http

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::rest
{

class Server
{
public:
    using ApiGatewaySingle = gsl::not_null<rest::ApiGateway*>;
    using AuthorizeMethodSingle = gsl::not_null<http::AuthorizeMethod*>;

    virtual ~Server() = default;

    virtual void run() = 0;
    virtual ApiGatewaySingle apiGateway() const = 0;
    virtual AuthorizeMethodSingle authorizeMethod() const = 0;
};

} // namespace mpk::rest
