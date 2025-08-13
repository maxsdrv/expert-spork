#pragma once

#include "Config.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"
#include "yaml/yamlConverter.h"

namespace mpk::dss::config
{

template <typename T>
struct Option
{
    T value;

    bool operator==(const Option& s) const
    {
        return value == s.value;
    }
    bool operator!=(const Option& s) const
    {
        return !(*this == s);
    }
};

} // namespace mpk::dss::config

template <typename T>
struct ToJsonConverter<mpk::dss::config::Option<T>>
{
    static QJsonValue convert(const mpk::dss::config::Option<T>& setting)
    {
        return json::toValue(setting);
    }
};

template <typename T>
struct FromJsonConverter<mpk::dss::config::Option<T>>
{
    static mpk::dss::config::Option<T> get(const QJsonValue& value)
    {
        return mpk::dss::config::Option<T>{json::fromValue<T>(value)};
    }
};

namespace YAML
{

using namespace mpk::dss::config;

template <typename T>
struct convert<Option<T>>
{
    static Node encode(const Option<T>& setting)
    {
        return Node(setting.value);
    }
    static bool decode(const Node& node, Option<T>& setting)
    {
        T value;
        if (!parseValue(node, value))
        {
            return false;
        }
        setting.value = value;
        return true;
    }
};

} // namespace YAML
