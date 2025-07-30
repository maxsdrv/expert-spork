#pragma once

#include "json/FromJson.h"

#include <QJsonArray>

template <>
struct FromJsonConverter<QJsonObject>
{
    static QJsonObject get(const QJsonValue& value)
    {
        if (!value.isObject())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::Object, value.type()));
        }
        return value.toObject();
    }
};

template <>
struct FromJsonConverter<QJsonArray>
{
    static QJsonArray get(const QJsonValue& value)
    {
        if (!value.isArray())
        {
            BOOST_THROW_EXCEPTION(
                exception::json::UnexpectedType(QJsonValue::Array, value.type()));
        }
        return value.toArray();
    }
};
