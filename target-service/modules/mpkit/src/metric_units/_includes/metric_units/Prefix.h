#pragma once

#include "exception/InvalidValue.h"
#include "exception/StringParseFailed.h"

#include "types/TypeList.h"

#include <QCoreApplication>

#include <cstring>

namespace mpk::metric_units
{

template <typename P1, typename P2, typename V>
V prefix_cast(V value) noexcept // NOLINT (readability-identifier-naming)
{
    constexpr auto order = P1::order - P2::order;
    constexpr auto mag = 10;
    if constexpr (order > 0)
    {
        for (auto x = order; x > 0; x--)
        {
            value *= mag;
        }
    }
    else if constexpr (order < 0)
    {
        for (auto x = order; x < 0; x++)
        {
            value /= mag;
        }
    }

    return value;
}

namespace prefix
{

template <typename P>
auto trName()
{
    return QCoreApplication::translate("mpk::metric_units", P::name);
}

struct Exa
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "E");
    static constexpr auto order = 18;
};

struct Peta
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "P");
    static constexpr auto order = 15;
};

struct Tera
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "T");
    static constexpr auto order = 12;
};

struct Giga
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "G");
    static constexpr auto order = 9;
};

struct Mega
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "M");
    static constexpr auto order = 6;
};

struct Kilo
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "k");
    static constexpr auto order = 3;
};

struct Hecto
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "h");
    static constexpr auto order = 2;
};

struct Deca
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "da");
    static constexpr auto order = 1;
};

struct None
{
    static constexpr auto name = "";
    static constexpr auto order = 0;
};

struct Deci
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "d");
    static constexpr auto order = -1;
};

struct Centi
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "c");
    static constexpr auto order = -2;
};

struct Milli
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "m");
    static constexpr auto order = -3;
};

struct Micro
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "u");
    static constexpr auto order = -6;
};

struct Nano
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "n");
    static constexpr auto order = -9;
};

struct Pico
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "p");
    static constexpr auto order = -12;
};

struct Femto
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "f");
    static constexpr auto order = -15;
};

struct Atto
{
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "a");
    static constexpr auto order = -18;
};

// clang-format off
using ListAll =
    typelist::ListT<Exa, Peta, Tera, Giga, Mega, Kilo, Hecto, Deca,
        None, Deci, Milli, Centi, Micro, Nano, Pico, Femto, Atto>;
// clang-format on
using ListStandard = typelist::ListT<Giga, Mega, Kilo, None, Milli, Micro, Nano>;
using ListNone = typelist::ListT<None>;

template <typename P, typename L, typename V, template <typename> typename N>
struct Converter
{
    static V fromName(V value, const QString& prefix)
    {
        using Prefix = typelist::HeadT<L>;
        if (prefix == N<Prefix>::value())
        {
            return prefix_cast<Prefix, P, V>(value);
        }
        return Converter<P, typelist::TailT<L>, V, N>::fromName(value, prefix);
    }
};

template <typename P, typename V, template <typename> typename N>
struct Converter<P, typelist::Nil, V, N>
{
    static V fromName(V /*value*/, const QString& prefix)
    {
        BOOST_THROW_EXCEPTION(exception::StringParseFailed<QString>(prefix));
    }
};

template <typename V>
struct NormalizedType
{
    V value;
    QString displayName;
};

template <typename P, typename L, typename V, template <typename> typename N>
struct Normalizer
{
    static NormalizedType<V> normalize(V value)
    {
        constexpr auto bottomThreshold = 1;
        constexpr auto topThreshold = 1000;

        using Prefix = typelist::HeadT<L>;
        auto resValue = prefix_cast<P, Prefix, V>(value);
        if (std::abs(resValue) >= bottomThreshold && std::abs(resValue) < topThreshold)
        {
            return {resValue, trName<Prefix>()};
        }
        return Normalizer<P, typelist::TailT<L>, V, N>::normalize(value);
    }
};

template <typename P, typename V, template <typename> typename N>
struct Normalizer<P, typelist::Nil, V, N>
{
    static NormalizedType<V> normalize(V value)
    {
        BOOST_THROW_EXCEPTION(
            exception::InvalidValue()
            << exception::ExceptionInfo("Unexpected value: " + std::to_string(value)));
    }
};

} // namespace prefix

} // namespace mpk::metric_units
