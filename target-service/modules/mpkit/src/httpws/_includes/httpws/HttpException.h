#pragma once

#include "exception/General.h"

#include "httpws/Definitions.h"
#include "httpws/HttpMethod.h"

#include <string>

namespace exception
{

namespace http
{

using HttpErrorInfo = boost::error_info<struct tag_response_status, int>;
using HttpMethodInfo = boost::error_info<struct tag_response_status, ::http::HttpMethod::Value>;

using ResponseStatus = boost::error_info<struct tag_response_status, ::http::HttpCode>;

using ResponseMessage = boost::error_info<struct tag_response_message, std::string>;

class Exception : public General
{
};

class UnsupportedMethod : public Exception
{
public:
    explicit UnsupportedMethod(::http::HttpMethod::Value method)
    {
        (*this) << HttpMethodInfo(method);
    }
};

template <typename E>
int httpErrorInfo(const E& ex)
{
    return exception::info<HttpErrorInfo>(ex, 0);
}

template <typename E>
::http::HttpCode responseErrorStatus(const E& ex)
{
    return exception::info<ResponseStatus>(ex, ::http::HttpCode::InternalServerError);
}

template <typename E>
std::string responseErrorMessage(const E& ex)
{
    return exception::info<ResponseMessage>(ex, std::string("Internal server error"));
}

} // namespace http

} // namespace exception
