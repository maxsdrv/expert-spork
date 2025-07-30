#pragma once

#include "json/ToJson.h"

#include "json/concepts/Arithmetic.h"
#include "json/concepts/QEnum.h"

#include "qttypes/MetaEnum.h"

#include <optional>

// Any value convertible to QJsonValue except big integers and Q_ENUMS
template <typename T>
struct ToJsonConverter<
    T,
    typename std::enable_if_t<
        std::is_convertible_v<T, QJsonValue> && !concepts::IsQEnumV<T> && !concepts::IsBigInt<T>,
        T>>
{
    static QJsonValue convert(const T& value)
    {
        return QJsonValue(value);
    }
};

// All 32-bit (or less) integral types, except for directly convertible
// unsigned integers cannot be converted to QJsonValue directly
template <typename T>
struct ToJsonConverter<
    T,
    typename std::enable_if_t<concepts::IsSmallInt<T> && !std::is_convertible_v<T, QJsonValue>, T>>
{
    static QJsonValue convert(T value)
    {
        return QJsonValue{static_cast<qint64>(value)};
    }
};

// 64-bit integral types
// 64-bit integers may overlap double precision, so we pass it as string
// Read QJsonValue::QJsonValue(qint64) description for more info
template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsBigInt<T>, T>>
{
    static QJsonValue convert(T value)
    {
        return json::toValue(QString::number(value));
    }
};

// Convert enum declared as Q_ENUM
template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsQEnumV<T>, T>>
{
    static QJsonValue convert(T enumValue)
    {
        return qt::metaEnumToString(enumValue);
    }
};

/// Any std::string convertible types (char* etc)
template <typename T>
struct ToJsonConverter<
    T,
    typename std::enable_if<std::is_convertible<T, std::string>::value, T>::type>
{
    static QJsonValue convert(const T& value)
    {
        return QJsonValue(strings::fromUtf8(value));
    }
};

template <typename T>
struct ToJsonConverter<std::optional<T>>
{
    static QJsonValue convert(const std::optional<T>& optional)
    {
        return optional.has_value() ? json::toValue(*optional) : QJsonValue();
    }
};

template <typename T, typename Format>
struct ToJsonConverterWithFormat<std::optional<T>, Format>
{
    static QJsonValue convert(const std::optional<T>& optional, const Format& format)
    {
        return optional.has_value() ? json::toValueWithFormat(*optional, format) : QJsonValue();
    }
};
