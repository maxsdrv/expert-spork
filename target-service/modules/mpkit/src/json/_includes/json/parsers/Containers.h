#pragma once

#include "json/FromJson.h"
#include "json/concepts/Containers.h"
#include "json/concepts/StdArray.h"

#include "json/parsers/QJsonTypes.h"

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsSequenceContainer<T>::value, T>>
{
    static T get(const QJsonValue& value)
    {
        auto array = json::fromValue<QJsonArray>(value);
        T container;
        auto inserter = concepts::IsSequenceContainer<T>::makeInserter(container);
        for (auto it = array.constBegin(); it != array.constEnd(); ++it)
        {
            try
            {
                inserter = json::fromValue<typename T::value_type>(*it);
            }
            catch (boost::exception& ex)
            {
                QString path = QString::number(std::distance(array.constBegin(), it));
                if (const QString* previousKey = boost::get_error_info<exception::json::Key>(ex))
                {
                    path.append(" > " + *previousKey);
                }
                ex << exception::json::Key(path);
                throw;
            }
        }
        return container;
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsAssociativeContainer<T>::value, T>>
{
    static T get(const QJsonValue& value)
    {
        if (!value.isObject())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::Object, value.type()));
        }

        auto object = value.toObject();
        T container;
        auto inserter = concepts::IsAssociativeContainer<T>::makeInserter(container);
        for (auto it = object.constBegin(); it != object.constEnd(); ++it)
        {
            try
            {
                inserter = std::pair{
                    json::fromValue<typename T::key_type>(it.key()),
                    json::fromValue<typename T::mapped_type>(it.value())};
            }
            catch (boost::exception& ex)
            {
                QString path = QString::number(std::distance(object.constBegin(), it));
                if (const QString* previousKey = boost::get_error_info<exception::json::Key>(ex))
                {
                    path.append(" > " + *previousKey);
                }
                ex << exception::json::Key(path);
                throw;
            }
        }
        return container;
    }
};

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsStdArrayV<T>, T>>
{
    static T get(const QJsonValue& value)
    {
        T array;
        constexpr auto size = static_cast<int>(array.size());
        auto jsonArray = json::fromValue<QJsonArray>(value);
        if (jsonArray.size() != size)
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedType(
                QStringLiteral("Array size mismatch, expected=%1, actual=%2")
                    .arg(size, jsonArray.size())));
        }

        for (auto i = 0; i < size; ++i)
        {
            try
            {
                array[i] = json::fromValue<typename T::value_type>(jsonArray.at(i));
            }
            catch (boost::exception& ex)
            {
                QString path = QString::number(i);
                if (const QString* previousKey = boost::get_error_info<exception::json::Key>(ex))
                {
                    path.append(" > " + *previousKey);
                }
                ex << exception::json::Key(path);
                throw;
            }
        }
        return array;
    }
};
