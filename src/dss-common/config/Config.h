#pragma once

#include "yaml/yamlConverter.h"

#include <sstream>
#include <string>

namespace mpk::dss::config
{

std::string expandEnvVar(const std::string& value);

template <typename T>
bool parseValue(const YAML::Node& node, T& result)
{
    std::istringstream parser(expandEnvVar(node.as<std::string>()));
    T value;
    parser >> value;
    if (parser.fail() || !parser.eof())
    {
        return false;
    }
    result = value;
    return true;
}

template <>
inline bool parseValue<char>(const YAML::Node& node, char& result)
{
    std::istringstream parser(expandEnvVar(node.as<std::string>()));
    std::string value;
    parser >> value;
    if (parser.fail() || !parser.eof() || value.size() != 1)
    {
        return false;
    }
    result = value[0];
    return true;
}

template <>
inline bool parseValue<std::string>(const YAML::Node& node, std::string& result)
{
    result = expandEnvVar(node.as<std::string>());
    return true;
}

} // namespace mpk::dss::config
