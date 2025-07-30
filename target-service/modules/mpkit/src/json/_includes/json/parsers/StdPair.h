#pragma once

#include "json/FromJson.h"

template <typename First, typename Second>
struct FromJsonConverter<std::pair<First, Second>>
{
    static std::pair<First, Second> get(const QJsonValue& value)
    {
        auto object = value.toObject();
        return {
            json::fromObject<First>(object, "first"), json::fromObject<Second>(object, "second")};
    }
};
