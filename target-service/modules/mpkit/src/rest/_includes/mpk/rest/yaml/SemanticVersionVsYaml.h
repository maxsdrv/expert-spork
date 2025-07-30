#pragma once

#include "mpk/rest/SemanticVersion.h"

#include "yaml/yamlConverter.h"

namespace YAML
{

template <>
struct convert<mpk::rest::SemanticVersion>
{
    using SemanticVersion = mpk::rest::SemanticVersion;

    static Node encode(const SemanticVersion& version);
    static bool decode(const Node& node, SemanticVersion& version);
};

} // namespace YAML
