#pragma once

#include "json/FromJson.h"

template <typename T>
struct FromJsonConverter<T, typename std::enable_if_t<std::is_convertible_v<T, QString>, T>>
{
    static T get(const QJsonValue& value)
    {
        if (!value.isString())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::String, value.type()));
        }
        return T{value.toString()};
    }
};
