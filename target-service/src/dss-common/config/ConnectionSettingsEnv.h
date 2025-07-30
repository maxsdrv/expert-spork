#pragma once

#include "httpws/ConnectionSettings.h"

#include <string>

namespace mpk::dss::config
{

struct ConnectionSettingsEnv : http::ConnectionSettings
{
    ConnectionSettingsEnv() = default;
    explicit ConnectionSettingsEnv(const std::string& envVarName);
};

} // namespace mpk::dss::config
