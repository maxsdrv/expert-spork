#pragma once

#include "mpk/rest/ApiDescription.h"

#include "httpws/Permissions.h"

#include <QJsonObject>

#include <filesystem>

namespace mpk::rest
{

class ApiDescriptionJsonFile final : public ApiDescription
{
public:
    explicit ApiDescriptionJsonFile(const std::filesystem::path& filename);

private:
    http::HttpRoute route(const QString& path) const override;
    http::HttpRoute route(const QString& path, http::HttpMethod::Value method) const override;

private:
    using HttpMethod = http::HttpMethod::Value;
    using Permissions = http::Permissions;

    using PermissionsByMethod = std::unordered_map<HttpMethod, Permissions>;
    using MethodsByPath = std::unordered_map<QString, PermissionsByMethod>;

    MethodsByPath::const_iterator findPath(const QString& path) const;

    static ApiDescriptionJsonFile::MethodsByPath load(const std::filesystem::path& filename);

private:
    MethodsByPath m_paths;
};

} // namespace mpk::rest
