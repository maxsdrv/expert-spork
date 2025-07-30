#pragma once

#include "json/FromJson.h"
#include "json/concepts/QMap.h"
#include "json/parsers/QJsonTypes.h"
#include "json/parsers/StdPair.h"

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<concepts::IsQMap<T>::value, T>>
{
    static T get(const QJsonValue& value)
    {
        auto array = json::fromValue<QJsonArray>(value);
        T map;
        for (auto it = array.constBegin(); it != array.constEnd(); ++it)
        {
            try
            {
                const auto pair =
                    json::fromValue<std::pair<typename T::key_type, typename T::mapped_type>>(
                        *it);
                map.insert(pair.first, pair.second);
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
        return map;
    }
};
