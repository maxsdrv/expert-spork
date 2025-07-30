#pragma once

#include <cstdint>

namespace http
{

constexpr auto API_VERSION = 1;

constexpr auto jsonContentType = "application/json";
constexpr auto textContentType = "text/plain";
constexpr auto contentTypeHeader = "Content-Type";

constexpr auto loginPath = "login";
constexpr auto registerPath = "register";

enum class HttpCode : std::uint16_t
{
    Ok = 200,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotAcceptable = 406,
    UnsupportedMediaType = 415,
    UnprocessableEntity = 422,
    InternalServerError = 500,
    NotImplemented = 501,
    ServiceUnavailable = 503
};

} // namespace http
