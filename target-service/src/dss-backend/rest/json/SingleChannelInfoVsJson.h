#pragma once

#include "dss-backend/rest/channel/SingleChannelInfo.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::SingleChannelInfo>
{
    static QJsonValue convert(
        const mpk::dss::backend::rest::SingleChannelInfo& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::SingleChannelInfo>
{
    static mpk::dss::backend::rest::SingleChannelInfo get(
        const QJsonValue& value);
};
