#pragma once

#include "UserPermissionsProvider.h"

#include <filesystem>

namespace mpk::rest
{

class UserPermissionsJsonFile : public UserPermissionsProvider
{
public:
    explicit UserPermissionsJsonFile(std::filesystem::path path);

    http::Permissions list(const QString& username = QString()) const override;

private:
    std::filesystem::path m_path;
};

} // namespace mpk::rest
