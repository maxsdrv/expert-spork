#pragma once

#include "json/ToJson.h"

#include <QJsonObject>

template <typename First, typename Second>
struct ToJsonConverter<std::pair<First, Second>>
{
    static QJsonValue convert(const std::pair<First, Second>& pair)
    {
        return QJsonObject{
            {"first", json::toValue(pair.first)}, {"second", json::toValue(pair.second)}};
    }
};
