#pragma once

#include "dss-backend/rest/geolocation/Geolocation.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::Geolocation>
{
    static QJsonValue convert(const mpk::dss::backend::rest::Geolocation& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::Geolocation>
{
    static mpk::dss::backend::rest::Geolocation get(const QJsonValue& value);
};
