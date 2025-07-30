#pragma once

#include "json/ToJson.h"
#include "json/composers/General.h"

template <>
struct ToJsonConverter<QUrl>
{
    static QJsonValue convert(QUrl const& url)
    {
        return json::toValue(url.toString());
    }
};
