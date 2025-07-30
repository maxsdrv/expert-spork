#pragma once

#include "httpws/Permissions.h"

namespace mpk::rest
{

class UserPermissionsProvider
{
public:
    virtual ~UserPermissionsProvider() = default;
    virtual http::Permissions list(const QString& username = QString()) const = 0;
};

} // namespace mpk::rest
