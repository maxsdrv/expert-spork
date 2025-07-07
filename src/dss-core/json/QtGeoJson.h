#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include <QGeoCoordinate>
#include <QGeoPolygon>

template <>
struct ToJsonConverter<QGeoCoordinate>
{
    static QJsonValue convert(const QGeoCoordinate& coord);
};

template <>
struct FromJsonConverter<QGeoCoordinate>
{
    static QGeoCoordinate get(const QJsonValue& value);
};

template <>
struct ToJsonConverter<QGeoPolygon>
{
    static QJsonValue convert(const QGeoPolygon& polygon);
};

template <>
struct FromJsonConverter<QGeoPolygon>
{
    static QGeoPolygon get(const QJsonValue& value);
};
