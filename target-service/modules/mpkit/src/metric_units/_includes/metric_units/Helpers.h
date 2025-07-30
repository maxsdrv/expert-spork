#pragma once

#include <QChar>

#include <functional>

#include "Prefix.h"

namespace mpk::metric_units
{

namespace helpers
{

template <typename P>
struct Name
{
    static auto value()
    {
        return QString(P::name);
    }
};

template <typename P>
struct DisplayName
{
    static auto value()
    {
        return prefix::trName<P>();
    }
};

static constexpr auto point() noexcept
{
    return '.';
}

static inline QString chopTrailingZeros(const QString& string)
{
    if (string.lastIndexOf(point()) > 0)
    {
        auto chopped = string;
        while (chopped.endsWith('0'))
        {
            chopped.chop(1);
        }
        if (chopped.endsWith(point()))
        {
            chopped.chop(1);
        }
        return chopped;
    }
    return string;
}

} // namespace helpers

namespace numeric_validator
{

using NumericValidationFunc = std::function<bool(const QChar& character)>;

static constexpr auto allDouble(const QChar& ch)
{
    return ch.isNumber() || ch == helpers::point() || ch == '-' || ch == '+' || ch == 'e';
};

static constexpr auto pointOnly(const QChar& ch)
{
    return ch.isNumber() || ch == helpers::point();
};

} // namespace numeric_validator

} // namespace mpk::metric_units
