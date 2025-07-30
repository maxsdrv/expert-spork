#pragma once

#include "dss-backend/rest/diagnostic/DeviceDiagnosticData.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

template <>
struct ToJsonConverter<mpk::dss::backend::rest::DeviceDiagnosticData>
{
    static QJsonValue convert(
        const mpk::dss::backend::rest::DeviceDiagnosticData& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::rest::DeviceDiagnosticData>
{
    static mpk::dss::backend::rest::DeviceDiagnosticData get(
        const QJsonValue& value);
};
