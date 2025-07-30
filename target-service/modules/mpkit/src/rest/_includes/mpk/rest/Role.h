#pragma once

#include <QString>

#include <map>
#include <set>

namespace mpk::rest
{

using Permission = QString;
using Permissions = std::set<Permission>;
using Role = QString;
using Roles = std::map<Role, Permissions>;

} // namespace mpk::rest
