#pragma once

#include "mpk/stable_link/PolicySettings.h"

#include "yaml/yamlConverter.h"

namespace YAML
{

template <>
struct convert<mpk::stable_link::PolicySettings>
{
    using PolicySettings = mpk::stable_link::PolicySettings;

    static Node encode(const PolicySettings& policy);
    static bool decode(const Node& node, PolicySettings& policy);
};

} // namespace YAML
