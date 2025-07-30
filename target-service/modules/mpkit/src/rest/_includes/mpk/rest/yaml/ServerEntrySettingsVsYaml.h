#pragma once

#include "mpk/rest/ServerEntrySettings.h"
#include "yaml/yamlConverter.h"

namespace YAML
{

template <>
struct convert<mpk::rest::ServerEntrySettings>
{
    using ServerEntrySettings = mpk::rest::ServerEntrySettings;

    static Node encode(const ServerEntrySettings& settings);
    static bool decode(const Node& node, ServerEntrySettings& settings);
};

} // namespace YAML
