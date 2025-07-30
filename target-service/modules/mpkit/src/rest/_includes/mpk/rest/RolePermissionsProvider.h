#pragma once

#include "Role.h"

namespace mpk::rest
{

class RolePermissionsProvider
{
public:
    virtual ~RolePermissionsProvider() = default;

    virtual Permissions permissions(const QString& role = QString()) const = 0;
};

} // namespace mpk::rest
