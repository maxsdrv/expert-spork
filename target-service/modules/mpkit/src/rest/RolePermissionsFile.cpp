#include "mpk/rest/RolePermissionsFile.h"

#include "json/JsonParser.h"
#include "json/JsonUtilities.h"

namespace
{

constexpr auto permissionsTag = "permissions";

}

namespace mpk::rest
{

RolePermissionsFile::RolePermissionsFile(const QString& filename)
{
    load(filename);
}

Permissions RolePermissionsFile::permissions(const QString& role) const
{
    const auto it = m_roles.find(role);
    return (it != m_roles.end() ? it->second : Permissions{});
}

void RolePermissionsFile::load(const QString& filename)
{
    m_roles.clear();
    auto file = json::fromFile(filename);
    auto keys = file.keys();
    for (const auto& key: keys)
    {
        auto permissionsJson = file.value(key).toObject();
        auto permissions = json::fromObject<Permissions>(permissionsJson, permissionsTag);
        m_roles.emplace(key, permissions);
    }
}

} // namespace mpk::rest
