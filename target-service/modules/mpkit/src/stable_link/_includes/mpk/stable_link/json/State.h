#pragma once

#include "mpk/stable_link/State.h"

#include "json/FromJson.h"
#include "json/ToJson.h"

template <>
struct ToJsonConverter<mpk::stable_link::State>
{
    static QJsonValue convert(mpk::stable_link::State state);
};

template <>
struct FromJsonConverter<mpk::stable_link::State>
{
    static mpk::stable_link::State get(const QJsonValue& value);
};
