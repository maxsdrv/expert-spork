#pragma once

#include "httpws/ConnectionSettings.h"

#include <filesystem>
#include <optional>

namespace mpk::rest
{

struct ApiGatewaySettings
{
    http::ConnectionSettings connectionSettings;

    std::filesystem::path passwdFile;
    std::filesystem::path authFile;
    std::filesystem::path apiDescriptionFile;
    std::optional<std::filesystem::path> userPermissionsFile;
};

} // namespace mpk::rest
