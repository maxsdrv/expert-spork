#pragma once

#include "dss-backend/rest/channel/ChannelInfo.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::ChannelInfo>
{
    static QJsonValue convert(const mpk::dss::backend::rest::ChannelInfo& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::ChannelInfo>
{
    static mpk::dss::backend::rest::ChannelInfo get(const QJsonValue& value);
};
