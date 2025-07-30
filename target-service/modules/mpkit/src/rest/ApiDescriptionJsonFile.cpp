#include "mpk/rest/ApiDescriptionJsonFile.h"

#include "httpws/HttpMethod.h"
#include "httpws/HttpRoute.h"
#include "httpws/Permissions.h"

#include "qt/Strings.h"

#include "json/JsonUtilities.h"

#include <exception/ErrorInfo.h>
#include <exception/InvalidValue.h>

namespace mpk::rest
{

namespace
{

bool isEqualPath(const QString& path, const QString& origin)
{
    auto pathItems = path.split("/");
    auto originItems = origin.split("/");
    if (pathItems.size() != originItems.size())
    {
        return false;
    }

    for (auto i = 0; i < originItems.size(); ++i)
    {
        if (pathItems[i] != originItems[i] && originItems[i] != "{id}")
        {
            return false;
        }
    }

    return true;
}

} // namespace

ApiDescriptionJsonFile::ApiDescriptionJsonFile(const std::filesystem::path& filename) :
  m_paths{load(filename)}
{
}

http::HttpRoute ApiDescriptionJsonFile::route(const QString& path) const
{
    auto itPath = findPath(path);
    if (itPath != m_paths.end())
    {
        const PermissionsByMethod& methods = itPath->second;
        if (!methods.empty())
        {
            const HttpMethod method = methods.begin()->first;
            const Permissions permissions = methods.begin()->second;

            return {method, path, permissions};
        }
    }

    BOOST_THROW_EXCEPTION(
        exception::InvalidValue() << exception::ExceptionInfo(strings::toUtf8(path)));
}

http::HttpRoute ApiDescriptionJsonFile::route(
    const QString& path, http::HttpMethod::Value method) const
{
    auto itPath = findPath(path);
    if (itPath != m_paths.end())
    {
        const PermissionsByMethod& methods = itPath->second;

        auto itMethod = methods.find(method);
        if (itMethod != methods.end())
        {
            const HttpMethod method = itMethod->first;
            const Permissions& permissions = itMethod->second;

            return {method, path, permissions};
        }
    }

    BOOST_THROW_EXCEPTION(
        exception::InvalidValue() << exception::ExceptionInfo(strings::toUtf8(path)));
}

ApiDescriptionJsonFile::MethodsByPath ApiDescriptionJsonFile::load(
    const std::filesystem::path& filename)
{
    ApiDescriptionJsonFile::MethodsByPath paths;

    auto file = json::fromFile(filename);
    auto keys = file.keys();
    for (const auto& key: keys)
    {
        ApiDescriptionJsonFile::PermissionsByMethod& methods = paths[key];

        auto pathJson = file.value(key).toObject();

        auto methodStr = pathJson.value("method").toString();
        auto method = http::HttpMethod::fromString(methodStr);

        auto permissionsStr = pathJson.value("permissions");
        auto permissions = json::fromValue<http::Permissions>(permissionsStr);

        methods[method] = permissions;
    }

    return paths;
}

auto ApiDescriptionJsonFile::findPath(const QString& path) const
    -> ApiDescriptionJsonFile::MethodsByPath::const_iterator
{
    auto itPath = m_paths.find(path);

    if (itPath == m_paths.end())
    {
        for (auto it = m_paths.begin(); it != m_paths.end(); ++it)
        {
            if (isEqualPath(path, it->first))
            {
                return it;
            }
        }
    }

    return itPath;
}

} // namespace mpk::rest
