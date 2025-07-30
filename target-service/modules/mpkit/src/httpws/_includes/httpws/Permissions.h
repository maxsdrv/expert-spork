#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include <QSet>
#include <QString>

namespace http
{

struct Permissions
{
    bool match(const Permissions& other) const;
    bool unauthorized() const;
    bool empty() const;

    using Group = QString;
    using Groups = QSet<Group>;
    Groups groups;

    static const Group UNAUTHORIZED;
};

} // namespace http

template <>
struct FromJsonConverter<http::Permissions>
{
    static http::Permissions get(const QJsonValue& json);
};

template <>
struct ToJsonConverter<http::Permissions>
{
    static QJsonValue convert(const http::Permissions& permissions);
};
