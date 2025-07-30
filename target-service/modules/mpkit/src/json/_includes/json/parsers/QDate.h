#pragma once

#include "json/FromJson.h"
#include "json/parsers/QString.h"

#include <QDateTime>

template <typename T>
struct FromJsonConverter<
    T,
    typename std::enable_if_t<std::is_same_v<T, QDate> || std::is_same_v<T, QDateTime>, T>>
{
    static T get(const QJsonValue& value)
    {
        const auto format = Qt::ISODate;
        return FromJsonConverterWithFormat<T, decltype(format)>::get(value, format);
    }
};

template <typename Type, typename Format>
struct FromJsonConverterWithFormat<
    Type,
    Format,
    typename std::enable_if_t<std::is_same_v<Type, QDate> || std::is_same_v<Type, QDateTime>, Type>>
{
    static Type get(const QJsonValue& value, const Format& format)
    {
        auto string = json::fromValue<QString>(value);
        if (string.isEmpty())
        {
            return Type();
        }
        auto dateTime = Type::fromString(string, format);
        if (!dateTime.isValid())
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(string));
        }
        return dateTime;
    }
};
