#pragma once

#include "json/ToJson.h"

#include "json/composers/StdPair.h"
#include "json/concepts/QMap.h"

#include <QJsonArray>

template <typename T>
struct ToJsonConverter<T, typename std::enable_if<concepts::IsQMapV<T>, T>::type>
{
    static QJsonValue convert(const T& map)
    {
        QJsonArray array;
        for (auto it = map.constKeyValueBegin(); it != map.constKeyValueEnd(); ++it)
        {
            array.push_back(json::toValue(*it));
        }
        return array;
    }
};
