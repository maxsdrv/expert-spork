#pragma once

#include "json/ToJson.h"
#include "json/concepts/Containers.h"
#include "json/concepts/StdArray.h"

#include <QJsonArray>
#include <QJsonObject>

template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsSequenceContainer<T>::value, T>>
{
    static QJsonValue convert(const T& container)
    {
        QJsonArray array;
        for (const auto& item: container)
        {
            array.push_back(json::toValue(item));
        }
        return array;
    }
};

template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsAssociativeContainer<T>::value, T>>
{
    static QJsonValue convert(const T& container)
    {
        QJsonObject object;
        for (const auto& [key, value]: container)
        {
            object.insert(strings::fromUtf8(key), json::guessValue(value));
        }
        return object;
    }
};

template <typename T>
struct ToJsonConverter<T, typename std::enable_if_t<concepts::IsStdArrayV<T>, T>>
{
    static QJsonValue convert(const T& stdArray)
    {
        QJsonArray array;
        for (size_t i = 0; i < stdArray.size(); ++i)
        {
            array.push_back(json::toValue(stdArray[i]));
        }
        return array;
    }
};
