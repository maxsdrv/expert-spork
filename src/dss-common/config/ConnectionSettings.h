#pragma once

#include "httpws/ConnectionSettings.h"
#include "httpws/json/ConnectionSettings.h"
#include "httpws/yaml/ConnectionSettings.h"
#include "json/JsonComposer.h"
#include "json/JsonParser.h"
#include "yaml/yamlConverter.h"

#include <QString>

namespace mpk::dss::config
{

struct ConnectionSettings : public http::ConnectionSettings
{
    ConnectionSettings() = default;
    explicit ConnectionSettings(const http::ConnectionSettings& cs)
    {
        host = cs.host;
        port = cs.port;
    }
};

} // namespace mpk::dss::config

template <>
struct ToJsonConverter<mpk::dss::config::ConnectionSettings>
{
    static QJsonValue convert(const mpk::dss::config::ConnectionSettings& cs)
    {
        return ToJsonConverter<http::ConnectionSettings>::convert(cs);
    }
};

template <>
struct FromJsonConverter<mpk::dss::config::ConnectionSettings>
{
    static mpk::dss::config::ConnectionSettings get(const QJsonValue& value)
    {
        return mpk::dss::config::ConnectionSettings(
            FromJsonConverter<http::ConnectionSettings>::get(value));
    }
};

namespace YAML
{

using namespace mpk::dss::config;

template <>
struct convert<mpk::dss::config::ConnectionSettings>
{
    static Node encode(const ConnectionSettings& cs)
    {
        return convert<http::ConnectionSettings>::encode(cs);
    }
    static bool decode(const Node& node, ConnectionSettings& cs);
};

} // namespace YAML
