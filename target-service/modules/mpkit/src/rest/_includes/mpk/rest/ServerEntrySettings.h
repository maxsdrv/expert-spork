#pragma once

#include "httpws/ConnectionSettings.h"

#include "mpk/rest/SemanticVersion.h"

#include <optional>

namespace mpk::rest
{

struct ServerEntrySettings
{
    http::ConnectionSettings httpAddress;
    QString login;

    std::optional<http::ConnectionSettings> broadcastAddress = std::nullopt;
    std::optional<SemanticVersion> requiredVersion = std::nullopt;
};

} // namespace mpk::rest
