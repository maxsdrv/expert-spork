#pragma once

#include "httpws/HttpMethod.h"

namespace http
{

struct HttpRoute;

} // namespace http

namespace mpk::rest
{

class ApiDescription
{
public:
    virtual ~ApiDescription() = default;

    virtual http::HttpRoute route(const QString& path) const = 0;
    virtual http::HttpRoute route(const QString& path, http::HttpMethod::Value method) const = 0;
};

} // namespace mpk::rest
