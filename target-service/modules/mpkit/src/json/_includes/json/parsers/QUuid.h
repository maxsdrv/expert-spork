#pragma once

#include "json/FromJson.h"
#include "json/parsers/QString.h"

template <>
struct FromJsonConverter<QUuid>
{
    static QUuid get(const QJsonValue& value)
    {
        return QUuid{json::fromValue<QString>(value)};
    }
};
