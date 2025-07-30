#pragma once

#include "json/FromJson.h"
#include "json/parsers/QString.h"

template <>
struct FromJsonConverter<QByteArray>
{
    static QByteArray get(const QJsonValue& value)
    {
        return QByteArray::fromBase64(json::fromValue<QString>(value).toLatin1());
    }
};
