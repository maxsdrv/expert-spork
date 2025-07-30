#pragma once

#include "JsonExceptions.h"

#include <QJsonValue>

template <typename T, typename U = T>
struct ToJsonConverter
{
};

template <typename Type, typename Format, typename U = Type>
struct ToJsonConverterWithFormat
{
};

namespace json
{
template <typename T>
[[nodiscard]] inline QJsonValue toValue(const T& value)
{
    return ToJsonConverter<T>::convert(value);
}

template <typename Type, typename Format>
[[nodiscard]] inline QJsonValue toValueWithFormat(const Type& value, const Format& format)
{
    return ToJsonConverterWithFormat<Type, Format>::convert(value, format);
}

template <typename T>
[[nodiscard]] inline QJsonValue guessValue(const T& value)
{
    return toValue(value);
}

[[nodiscard]] inline QJsonValue guessValue(const QString& value)
{
    auto constexpr trueLiteral = "true";
    auto constexpr falseLiteral = "false";

    QString boolValue = value.toLower();
    if (boolValue == trueLiteral)
    {
        return QJsonValue(true);
    }
    if (boolValue == falseLiteral)
    {
        return QJsonValue(false);
    }

    bool isOk = false;
    double doubleValue = value.toDouble(&isOk);
    if (isOk)
    {
        return QJsonValue(doubleValue);
    }

    return QJsonValue(value);
}

[[nodiscard]] inline QJsonValue guessValue(const std::string& value)
{
    return guessValue(strings::fromUtf8(value));
}

} // namespace json
