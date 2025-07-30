#pragma once

#include "json/ToJson.h"

#include <type_traits>

template <typename Type>
struct ToJsonConverter<Type, typename std::enable_if_t<std::is_pointer_v<Type>, Type>>

{
    static QJsonValue convert(Type value)
    {
        return value != nullptr ? json::toValue(*value) : QJsonValue{};
    }
};

template <typename Type, typename Format>
struct ToJsonConverterWithFormat<
    Type,
    Format,
    typename std::enable_if_t<std::is_pointer_v<Type>, Type>>
{
    static QJsonValue convert(const Type& value, const Format& format)
    {
        return value != nullptr ? json::toValueWithFormat(*value, format) : QJsonValue{};
    }
};
