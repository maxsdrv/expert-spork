#include "mpk/rest/UserPermissionsStatic.h"

namespace mpk::rest
{

UserPermissionsStatic::UserPermissionsStatic(GroupsMap groups) : m_groups{std::move(groups)}
{
}

http::Permissions UserPermissionsStatic::list(const QString& username) const
{
    http::Permissions::Groups groups;
    auto keys = m_groups.keys();
    for (const auto& group: keys)
    {
        if (username.isEmpty() || m_groups[group].contains(username))
        {
            groups << group;
        }
    }
    return http::Permissions{groups};
}

} // namespace mpk::rest
