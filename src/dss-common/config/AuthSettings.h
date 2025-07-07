#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"
#include "yaml/yamlConverter.h"

#include <QString>

namespace mpk::dss::config
{

struct AuthSettings
{
    struct Tag
    {
        static constexpr auto username = "username";
        static constexpr auto password = "password";
    };

    QString username;
    QString password;

    bool operator==(const AuthSettings& s) const;
    bool operator!=(const AuthSettings& s) const;
};

} // namespace mpk::dss::config

template <>
struct ToJsonConverter<mpk::dss::config::AuthSettings>
{
    static QJsonValue convert(const mpk::dss::config::AuthSettings& settings);
};

template <>
struct FromJsonConverter<mpk::dss::config::AuthSettings>
{
    static mpk::dss::config::AuthSettings get(const QJsonValue& value);
};

namespace YAML
{

using namespace mpk::dss::config;

template <>
struct convert<AuthSettings>
{
    static Node encode(const AuthSettings& settings);
    static bool decode(const Node& node, AuthSettings& settings);
};

} // namespace YAML
