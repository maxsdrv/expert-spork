#pragma once

#include "json/ToJson.h"
#include "json/composers/General.h"

template <>
struct ToJsonConverter<QUuid>
{
    static QJsonValue convert(QUuid uid)
    {
        return json::toValue(uid.toString());
    }
};
