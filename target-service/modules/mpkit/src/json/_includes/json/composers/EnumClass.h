#pragma once

#include "json/ToJson.h"
#include "json/composers/General.h"
#include "json/concepts/EnumClass.h"

template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsEnumClassV<T>, T>>
{
    static QJsonValue convert(T value)
    {
        return json::toValue(static_cast<int>(value));
    }
};
