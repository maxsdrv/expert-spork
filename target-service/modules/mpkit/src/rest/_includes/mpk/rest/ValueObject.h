#pragma once

#include "httpws/JsonRestUtils.h"

#include "json/FromJson.h"
#include "json/ToJson.h"

namespace mpk::rest
{

template <typename T>
struct ValueObject
{
    T value;
};

} // namespace mpk::rest

template <typename T>
struct FromJsonConverter<mpk::rest::ValueObject<T>>
{
    static mpk::rest::ValueObject<T> get(const QJsonValue& value)
    {
        return {.value = json::fromValueObject<T>(value.toObject())};
    }
};

template <typename T>
struct ToJsonConverter<mpk::rest::ValueObject<T>>
{
    static QJsonValue convert(const mpk::rest::ValueObject<T>& value)
    {
        return json::toValueObject(value.value);
    }
};
