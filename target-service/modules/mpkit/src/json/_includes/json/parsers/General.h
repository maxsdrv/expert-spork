#pragma once

#include "json/FromJson.h"
#include "json/concepts/Arithmetic.h"
#include "json/concepts/QEnum.h"
#include "json/parsers/QString.h"

#include "qttypes/MetaEnum.h"

template <typename T>
constexpr auto ParseAsNumber =
    (concepts::IsSmallInt<T> && !std::is_same_v<bool, T>) || std::is_floating_point_v<T>;

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<ParseAsNumber<T>, T>>
{
    static T get(const QJsonValue& value)
    {
        if (!value.isDouble())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::Double, value.type()));
        }

        return static_cast<T>(value.toDouble());
    }
};

template <>
struct FromJsonConverter<bool>
{
    static bool get(const QJsonValue& value)
    {
        if (value.isBool())
        {
            return value.toBool();
        }

        if (value.isDouble())
        {
            return static_cast<bool>(value.toInt());
        }

        BOOST_THROW_EXCEPTION(
            exception::json::UnexpectedType({QJsonValue::Bool, QJsonValue::Double}, value.type()));
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsSignedBigInt<T>, T>>
{
    static T get(const QJsonValue& value)
    {
        if (!value.isString())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::String, value.type()));
        }

        auto ok = false;
        auto integer = value.toString().toLongLong(&ok);
        if (!ok)
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(value.toString()));
        }
        return static_cast<T>(integer);
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsUnsignedBigInt<T>, T>>
{
    static T get(const QJsonValue& value)
    {
        if (!value.isString())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::String, value.type()));
        }

        auto ok = false;
        auto integer = value.toString().toULongLong(&ok);
        if (!ok)
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(value.toString()));
        }
        return static_cast<T>(integer);
    }
};

template <typename T>
struct FromJsonConverter<std::optional<T>>
{
    static std::optional<T> get(const QJsonValue& value)
    {
        return !(value.isNull() || value.isUndefined()) ?
                   std::optional<T>{json::fromValue<T>(value)} :
                   std::optional<T>{};
    }
};

template <typename T, typename Format>
struct FromJsonConverterWithFormat<std::optional<T>, Format>
{
    static std::optional<T> get(const QJsonValue& value, const Format& format)
    {
        return !(value.isNull() || value.isUndefined()) ?
                   std::optional<T>{json::fromValueWithFormat<T>(value, format)} :
                   std::optional<T>{};
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsQEnumV<T>, T>>
{
    static T get(const QJsonValue& jsonValue)
    {
        if (jsonValue.isDouble())
        {
            return static_cast<T>(jsonValue.toDouble());
        }

        return qt::metaEnumFromString<T>(json::fromValue<QString>(jsonValue));
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<std::is_convertible_v<T, std::string>, T>>
{
    static T get(const QJsonValue& value)
    {
        bool isConvertibleToString = value.isBool() || value.isDouble() || value.isString();

        if (isConvertibleToString)
        {
            return T{strings::toUtf8(value.toVariant().toString())};
        }

        BOOST_THROW_EXCEPTION(exception::json::UnexpectedType(
            {QJsonValue::Bool, QJsonValue::Double, QJsonValue::String}, value.type()));
    }
};
