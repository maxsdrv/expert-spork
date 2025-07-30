#pragma once

#include "mpk/rest/EntrySettings.h"

#include "yaml/yamlConverter.h"

namespace YAML
{

template <>
struct convert<mpk::rest::EntrySettings>
{
    using EntrySettings = mpk::rest::EntrySettings;

    static Node encode(const EntrySettings& value);
    static bool decode(const Node& node, EntrySettings& value);
};

} // namespace YAML
