#pragma once

#include "Permissions.h"

class QString;

namespace http
{

class AuthorizeMethod
{
public:
    virtual ~AuthorizeMethod() = default;
    virtual Permissions validate(const QString& accessTokenValue) const = 0;
};

} // namespace http
