#pragma once

#include "json/FromJson.h"
#include "json/ObjectParser.h"
#include "json/concepts/QtGeometry.h"
#include "json/parsers/General.h"
#include "json/parsers/QJsonTypes.h"

/**
 * {
 *  "x1": <underlying type>
 *  "y1": <underlying type>
 *  "x2": <underlying type>
 *  "y2": <underlying type>
 * }
 */
template <mpk::concepts::QtLine R>
struct FromJsonConverter<R>
{
    static R get(const QJsonValue& value)
    {
        using ValueType = std::result_of_t<decltype (&R::x1)(R)>;

        auto parser = json::ObjectParser{value};
        return R{
            parser.get<ValueType>(QStringLiteral("x1")),
            parser.get<ValueType>(QStringLiteral("y1")),
            parser.get<ValueType>(QStringLiteral("x2")),
            parser.get<ValueType>(QStringLiteral("y2"))};
    }
};

/**
 * {
 *  "x": <underlying type>
 *  "y": <underlying type>
 * }
 */
template <mpk::concepts::QtPoint P>
struct FromJsonConverter<P>
{
    static P get(const QJsonValue& value)
    {
        using ValueType = std::result_of_t<decltype (&P::x)(P)>;

        auto parser = json::ObjectParser{value};
        return P{
            parser.get<ValueType>(QStringLiteral("x")),
            parser.get<ValueType>(QStringLiteral("y"))};
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
struct FromJsonConverter<R>
{
    static R get(const QJsonValue& value)
    {
        using ValueType = std::result_of_t<decltype (&R::x)(R)>;

        auto parser = json::ObjectParser{value};
        auto rect =
            R{parser.get<ValueType>(QStringLiteral("x")),
              parser.get<ValueType>(QStringLiteral("y")),
              parser.get<ValueType>(QStringLiteral("width")),
              parser.get<ValueType>(QStringLiteral("height"))};
        if (!rect.isValid())
        {
            BOOST_THROW_EXCEPTION(exception::json::InvalidValue());
        }
        return rect;
    }
};

/**
 * {
 *  "width": <underlying type>
 *  "height": <underlying type>
 * }
 */
template <mpk::concepts::QtSize S>
struct FromJsonConverter<S>
{
    static S get(const QJsonValue& value)
    {
        using ValueType = std::result_of_t<decltype (&S::width)(S)>;

        auto parser = json::ObjectParser{value};
        return S{
            parser.get<ValueType>(QStringLiteral("width")),
            parser.get<ValueType>(QStringLiteral("height"))};
    }
};
