#include "AuthSettings.h"

#include "Config.h"

namespace mpk::dss::config
{

bool AuthSettings::operator==(const AuthSettings& s) const
{
    return std::tie(username, password) == std::tie(s.username, s.password);
}

bool AuthSettings::operator!=(const AuthSettings& s) const
{
    return !(*this == s);
}

} // namespace mpk::dss::config

QJsonValue ToJsonConverter<mpk::dss::config::AuthSettings>::convert(
    const mpk::dss::config::AuthSettings& settings)
{
    using Tag = mpk::dss::config::AuthSettings::Tag;
    return QJsonObject{
        {Tag::username, json::toValue(settings.username)},
        {Tag::password, json::toValue(settings.password)}};
}

mpk::dss::config::AuthSettings FromJsonConverter<
    mpk::dss::config::AuthSettings>::get(const QJsonValue& value)
{
    using Tag = mpk::dss::config::AuthSettings::Tag;
    auto object = json::fromValue<QJsonObject>(value);
    auto username = json::fromObject<QString>(object, Tag::username);
    auto password = json::fromObject<QString>(object, Tag::password);
    return mpk::dss::config::AuthSettings{username, password};
}

namespace YAML
{

Node convert<AuthSettings>::encode(const AuthSettings& settings)
{
    Node node;
    node[AuthSettings::Tag::username] = settings.username;
    node[AuthSettings::Tag::password] = settings.password;
    return node;
}

bool convert<AuthSettings>::decode(const Node& node, AuthSettings& settings)
{
    std::string value;

    if (!parseValue(node[AuthSettings::Tag::username], value))
    {
        return false;
    }
    settings.username = QString::fromStdString(value);

    if (!parseValue(node[AuthSettings::Tag::username], value))
    {
        return false;
    }
    settings.password = QString::fromStdString(value);

    return true;
}

} // namespace YAML
