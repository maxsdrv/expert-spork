#pragma once

#include "json/JsonObjectBuilder.h"
#include "json/ToJson.h"
#include "json/composers/General.h"
#include "json/concepts/QtGeometry.h"

/**
 * {
 *  "x1": <underlying type>
 *  "y1": <underlying type>
 *  "x2": <underlying type>
 *  "y2": <underlying type>
 * }
 */
template <mpk::concepts::QtLine R>
struct ToJsonConverter<R>
{
    static QJsonValue convert(const R& line)
    {
        return json::JsonObjectBuilder{}
            .add(QStringLiteral("x1"), line.x1())
            .add(QStringLiteral("y1"), line.y1())
            .add(QStringLiteral("x2"), line.x2())
            .add(QStringLiteral("y2"), line.y2())
            .template as<QJsonValue>();
    }
};

/**
 * {
 *  "x": <underlying type>
 *  "y": <underlying type>
 * }
 */
template <mpk::concepts::QtPoint P>
struct ToJsonConverter<P>
{
    static QJsonValue convert(const P& point)
    {
        return json::JsonObjectBuilder{}
            .add(QStringLiteral("x"), point.x())
            .add(QStringLiteral("y"), point.y())
            .template as<QJsonValue>();
    }
};

/**
 * {
 *  "x": <underlying type>
 *  "y": <underlying type>
 *  "width": <underlying type>
 *  "height": <underlying type>
 * }
 */
template <mpk::concepts::QtRect R>
struct ToJsonConverter<R>
{
    static QJsonValue convert(const R& rect)
    {
        return json::JsonObjectBuilder{}
            .add(QStringLiteral("x"), rect.x())
            .add(QStringLiteral("y"), rect.y())
            .add(QStringLiteral("width"), rect.width())
            .add(QStringLiteral("height"), rect.height())
            .template as<QJsonValue>();
    }
};

/**
 * {
 *  "width": <underlying type>
 *  "height": <underlying type>
 * }
 */
template <mpk::concepts::QtSize S>
struct ToJsonConverter<S>
{
    static QJsonValue convert(const S& size)
    {
        return json::JsonObjectBuilder{}
            .add(QStringLiteral("width"), size.width())
            .add(QStringLiteral("height"), size.height())
            .template as<QJsonValue>();
    }
};
