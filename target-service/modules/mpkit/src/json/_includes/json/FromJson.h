#pragma once

#include "JsonExceptions.h"
#include "concepts/Optional.h"

#include <QJsonObject>
#include <QJsonValue>

#include <type_traits>

template <typename T, typename U = T>
struct FromJsonConverter
{
};

template <typename Type, typename Format, typename U = Type>
struct FromJsonConverterWithFormat
{
};

namespace json
{
template <typename T>
[[nodiscard]] inline T fromValue(const QJsonValue& json)
{
    return FromJsonConverter<std::decay_t<T>>::get(json);
}

template <typename Type, typename Format>
[[nodiscard]] inline Type fromValueWithFormat(const QJsonValue& json, const Format& format)
{
    return FromJsonConverterWithFormat<Type, Format>::get(json, format);
}

inline bool hasValue(QJsonValue const& value)
{
    return !value.isUndefined() && !value.isNull();
}

template <typename T>
[[nodiscard]] inline T fromObject(const QJsonObject& json, const QString& key)
{
    auto path = QString{};
    try
    {
        auto value = json.value(key);
        if constexpr (concepts::IsOptionalV<T>)
        {
            if (!hasValue(value))
            {
                return std::nullopt;
            }
        }

        if (value.isUndefined())
        {
            BOOST_THROW_EXCEPTION(exception::json::MissingNode(key));
        }

        path = key;
        return fromValue<T>(value);
    }
    catch (boost::exception& ex)
    {
        if (QString const* previousKey = boost::get_error_info<exception::json::Key>(ex))
        {
            if (path.isEmpty())
            {
                path = *previousKey;
            }
            else
            {
                path.append(" > " + *previousKey);
            }
        }
        ex << exception::json::Key(path);
        throw;
    }
}

template <typename T>
[[nodiscard]] inline T fromObjectWithDefault(
    const QJsonObject& json, const QString& key, T const& defaultValue)
{
    auto value = json.value(key);
    if (!hasValue(value))
    {
        return defaultValue;
    }

    try
    {
        return fromValue<T>(value);
    }
    catch (boost::exception& ex)
    {
        QString path{key};
        if (QString const* previousKey = boost::get_error_info<exception::json::Key>(ex))
        {
            path.append(" > " + *previousKey);
        }
        ex << exception::json::Key(path);
        throw;
    }
}

template <typename Type, typename Format>
[[nodiscard]] inline Type fromObjectWithFormat(
    const QJsonObject& json, const QString& key, const Format& format)
{
    auto path = QString{};
    try
    {
        auto value = json.value(key);
        if constexpr (concepts::IsOptional<Type>())
        {
            if (!hasValue(value))
            {
                return std::nullopt;
            }
        }

        if (value.isUndefined())
        {
            BOOST_THROW_EXCEPTION(exception::json::MissingNode(key));
        }

        path = key;
        return fromValueWithFormat<Type>(value, format);
    }
    catch (boost::exception& ex)
    {
        if (QString const* previousKey = boost::get_error_info<exception::json::Key>(ex))
        {
            if (path.isEmpty())
            {
                path = *previousKey;
            }
            else
            {
                path.append(" > " + *previousKey);
            }
        }
        ex << exception::json::Key(path);
        throw;
    }
}

template <typename Type, typename Format>
[[nodiscard]] inline Type fromObjectWithFormat(
    const QJsonObject& json, const QString& key, const Format& format, const Type& defaultValue)
{
    auto value = json.value(key);
    if (!hasValue(value))
    {
        return defaultValue;
    }

    try
    {
        return fromValueWithFormat<Type>(value, format);
    }
    catch (boost::exception& ex)
    {
        QString path{key};
        if (QString const* previousKey = boost::get_error_info<exception::json::Key>(ex))
        {
            path.append(" > " + *previousKey);
        }
        ex << exception::json::Key(path);
        throw;
    }
}

} // namespace json
