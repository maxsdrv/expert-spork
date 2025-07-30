#pragma once

#include "RolePermissionsProvider.h"

namespace mpk::rest
{

class RolePermissionsFile : public RolePermissionsProvider
{
public:
    explicit RolePermissionsFile(const QString& filename);

private:
    Permissions permissions(const QString& role = QString()) const override;

private:
    void load(const QString& filename);

private:
    Roles m_roles;
};

} // namespace mpk::rest
