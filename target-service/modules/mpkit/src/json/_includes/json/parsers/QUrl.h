#pragma once

#include "json/FromJson.h"
#include "json/parsers/QString.h"

#include <QUrl>

template <>
struct FromJsonConverter<QUrl>
{
    static QUrl get(const QJsonValue& value)
    {
        return {json::fromValue<QString>(value)};
    }
};
