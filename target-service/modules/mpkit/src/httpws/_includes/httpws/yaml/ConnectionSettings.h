#pragma once

#include "httpws/ConnectionSettings.h"
#include "yaml/yamlConverter.h"

namespace YAML
{

template <>
struct convert<http::ConnectionSettings>
{
    static Node encode(const http::ConnectionSettings& settings);
    static bool decode(const Node& node, http::ConnectionSettings& settings);
};

} // namespace YAML
