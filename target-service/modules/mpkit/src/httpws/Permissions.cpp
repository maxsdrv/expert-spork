#include "httpws/Permissions.h"

namespace http
{

namespace
{

constexpr auto groupsTag = "groups";

} // namespace

const Permissions::Group Permissions::UNAUTHORIZED = "UNAUTHORIZED";

bool Permissions::match(const Permissions& other) const
{
    return groups.intersects(other.groups);
}

bool Permissions::unauthorized() const
{
    return groups.contains(Permissions::UNAUTHORIZED);
}

bool Permissions::empty() const
{
    return groups.empty();
}

} // namespace http

using namespace http;

Permissions FromJsonConverter<Permissions>::get(const QJsonValue& json)
{
    auto object = json::fromValue<QJsonObject>(json);
    auto groups = json::fromObject<QSet<QString>>(object, groupsTag);
    return Permissions{groups};
}

QJsonValue ToJsonConverter<Permissions>::convert(const Permissions& permissions)
{
    return QJsonObject{{groupsTag, json::toValue(permissions.groups)}};
}
