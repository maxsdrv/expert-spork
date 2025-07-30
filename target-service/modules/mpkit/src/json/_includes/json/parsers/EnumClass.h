#pragma once

#include "json/FromJson.h"
#include "json/concepts/EnumClass.h"
#include "json/parsers/General.h"

// Converts integer to enum class value.
// Note: this function is not very safe, better use magic_enum library
template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsEnumClassV<T>, T>>
{
    static T get(const QJsonValue& jsonValue)
    {
        return static_cast<T>(json::fromValue<int>(jsonValue));
    }
};
