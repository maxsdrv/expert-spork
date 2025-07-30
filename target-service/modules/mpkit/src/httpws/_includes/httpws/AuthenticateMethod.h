#pragma once

#include "AuthTokens.h"

#include <QString>

namespace http
{

struct AuthTokens;
struct Credentials;
class AuthenticateMethod
{
public:
    virtual ~AuthenticateMethod() = default;

    /**
     * Get token pair by supplied username and password
     */
    virtual AuthTokens getTokens(const Credentials& credentials) = 0;

    /**
     * Get token pair by supplied refresh token
     */
    virtual AuthTokens getTokens(const QString& refreshTokenValue) = 0;
};

} // namespace http
