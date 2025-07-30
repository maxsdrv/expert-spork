#pragma once

#include "json/ToJson.h"
#include "json/composers/General.h"

#include <QDateTime>

template <typename T>
struct ToJsonConverter<
    T,
    typename std::enable_if_t<std::is_same_v<T, QDate> || std::is_same_v<T, QDateTime>, T>>
{
    static QJsonValue convert(T const& dateTime)
    {
        return json::toValue(dateTime.toString(Qt::ISODate));
    }
};

template <typename Type, typename Format>
struct ToJsonConverterWithFormat<
    Type,
    Format,
    typename std::enable_if_t<std::is_same_v<Type, QDate> || std::is_same_v<Type, QDateTime>, Type>>
{
    static QJsonValue convert(const Type& dateTime, const Format& format)
    {
        return json::toValue(dateTime.toString(format));
    }
};
