#pragma once

#include "httpws/ConnectionSettings.h"

#include <optional>

namespace mpk::rest
{

struct EntrySettings
{
    using ConnectionSettings = http::ConnectionSettings;
    using ConnectionSettingsOpt = std::optional<ConnectionSettings>;

    ConnectionSettingsOpt httpServer{};
    ConnectionSettingsOpt broadcastServer{};
};

} // namespace mpk::rest
