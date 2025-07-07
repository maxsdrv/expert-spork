#pragma once

#include "dss-backend/rest/channel/ViewAngle.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::ViewAngle>
{
    static QJsonValue convert(const mpk::dss::backend::rest::ViewAngle& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::ViewAngle>
{
    static mpk::dss::backend::rest::ViewAngle get(const QJsonValue& value);
};
