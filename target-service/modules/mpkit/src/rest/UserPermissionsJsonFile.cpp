#include "mpk/rest/UserPermissionsJsonFile.h"

#include "json/JsonParser.h"
#include "json/JsonUtilities.h"

#include <boost/algorithm/cxx11/any_of.hpp>

namespace mpk::rest
{

UserPermissionsJsonFile::UserPermissionsJsonFile(std::filesystem::path path) :
  m_path{std::move(path)}
{
}

http::Permissions UserPermissionsJsonFile::list(const QString& username) const
{
    http::Permissions::Groups groups;
    auto object = json::fromFile(m_path);
    for (auto it = object.begin(); it != object.end(); ++it)
    {
        auto users = (*it).toArray();
        if (username.isEmpty()
            || boost::algorithm::any_of(
                users, [username](const auto& user) { return user.toString() == username; }))
        {
            groups << it.key();
        }
    }
    return http::Permissions{groups};
}

} // namespace mpk::rest
