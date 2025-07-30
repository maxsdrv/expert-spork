#pragma once

#include "exception/StringParseFailed.h"

#include "metric_units/Helpers.h"
#include "metric_units/Prefix.h"

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/JsonComposer.hpp"
#include "types/strong_typedef/json/JsonParser.hpp"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

namespace mpk::metric_units
{

/*
struct FrequencyTraits
{
    using Type = uint64;
    using Prefix = prefix::None;
    using PrefixList = prefix::ListAll;
    static constexpr auto name = "Hz";
    static constexpr auto displayName = QT_TR_NOOP("Hz");
    static inline const auto numericValidate = numeric_validator::allDouble;
}
*/

struct MetricUnitTag
{
    static constexpr auto value() noexcept
    {
        return "value";
    }
};

template <typename Traits>
class MetricUnit : public types::StrongTypedef<MetricUnit<Traits>, Traits>,
                   public types::Ordered<MetricUnit<Traits>>,
                   public types::Additive<MetricUnit<Traits>>,
                   public types::Scalable<MetricUnit<Traits>>,
                   public types::SerializableAsJsonObject<MetricUnit<Traits>, MetricUnitTag>
{
public:
    using StrongType = types::StrongTypedef<MetricUnit<Traits>, Traits>;
    using ValueType = typename Traits::ValueType;
    using ValuePrefix = typename Traits::Prefix;
    using PrefixList = typename Traits::PrefixList;
    using StrongType::value;
    using types::StrongTypedef<MetricUnit<Traits>, Traits>::StrongTypedef;

    template <typename TargetPrefix, typename ReturnType = double>
    constexpr ReturnType as() const noexcept
    {
        return prefix_cast<ValuePrefix, TargetPrefix, ReturnType>(
            static_cast<ReturnType>(value()));
    }

    template <typename TargetPrefix = ValuePrefix, bool printUnits = true>
    QString toString(int precision = 2, bool omitTrailingZeros = false) const noexcept
    {
        return toStringHelper<helpers::Name, TargetPrefix, printUnits>(
            precision, omitTrailingZeros);
    }

    template <typename TargetPrefix = ValuePrefix, bool printUnits = true>
    QString toDisplayString(int precision = 2, bool omitTrailingZeros = false) const noexcept
    {
        return toStringHelper<helpers::DisplayName, TargetPrefix, printUnits>(
            precision, omitTrailingZeros);
    }

    template <bool printUnits = true>
    QString toNormalizedString(int precision = 2, bool omitTrailingZeros = false) const noexcept
    {
        return toNormalizedStringHelper<helpers::DisplayName, printUnits>(
            precision, omitTrailingZeros);
    }

    template <typename DefaultPrefix = typename Traits::Prefix>
    constexpr static MetricUnit fromString(const QString& string)
    {
        return fromStringHelper<helpers::Name, DefaultPrefix>(string);
    }

    template <typename DefaultPrefix = typename Traits::Prefix>
    constexpr static MetricUnit fromDisplayString(const QString& string)
    {
        return fromStringHelper<helpers::DisplayName, DefaultPrefix>(string);
    }

    template <typename Prefix = typename Traits::Prefix, typename Type = ValueType>
    constexpr static MetricUnit fromValue(const Type& value)
    {
        return MetricUnit{prefix_cast<Prefix, ValuePrefix, Type>(value)};
    }

private:
    template <template <typename> typename N, typename TargetPrefix, bool printUnits>
    QString toStringHelper(int precision, bool omitTrailingZeros) const noexcept
    {
        auto fpValue = static_cast<double>(value());
        auto converted = prefix_cast<ValuePrefix, TargetPrefix, double>(fpValue);
        auto str = QString::number(converted, 'f', precision);
        if (precision > 0 && omitTrailingZeros)
        {
            str = helpers::chopTrailingZeros(str);
        }
        if constexpr (printUnits)
        {
            return str + " " + N<TargetPrefix>::value() + N<Traits>::value();
        }
        return str;
    }

    template <template <typename> typename N, bool printUnits>
    QString toNormalizedStringHelper(int precision, bool omitTrailingZeros) const noexcept
    {
        auto fpValue = static_cast<double>(value());
        auto normalized =
            fpValue != 0 ?
                prefix::Normalizer<ValuePrefix, PrefixList, double, N>::normalize(fpValue) :
                prefix::NormalizedType<double>{fpValue, prefix::trName<ValuePrefix>()};
        auto str = QString::number(normalized.value, 'f', precision);
        if (precision > 0 && omitTrailingZeros)
        {
            str = helpers::chopTrailingZeros(str);
        }
        if constexpr (printUnits)
        {
            return str + " " + normalized.displayName + N<Traits>::value();
        }
        return str;
    }

    template <template <typename> typename N, typename DefaultPrefix = typename Traits::Prefix>
    static MetricUnit fromStringHelper(const QString& string)
    {
        int index = 0;
        while (index < string.size() && (*Traits::numericValidate)(string.at(index)))
        {
            index++;
        }

        auto numPart = string.left(index);
        auto unitPart = string.mid(index).trimmed();

        auto value = convert(numPart);

        auto converted = 0.;
        if (unitPart.isEmpty())
        {
            // No units supplied using default
            converted = prefix_cast<DefaultPrefix, ValuePrefix, double>(value);
        }
        else
        {
            index = unitPart.indexOf(N<Traits>::value());
            if (index < 0)
            {
                BOOST_THROW_EXCEPTION(exception::StringParseFailed<QString>(string));
            }

            if (index == 0)
            {
                // No prefix for units
                converted = prefix_cast<prefix::None, ValuePrefix, double>(value);
            }
            else
            {
                converted = prefix::Converter<ValuePrefix, PrefixList, double, N>::fromName(
                    value, unitPart.left(index));
            }
        }

        return MetricUnit{static_cast<ValueType>(converted)};
    }

    static double convert(const QString& string)
    {
        auto ok = false;
        auto value = string.toDouble(&ok);
        if (!ok)
        {
            BOOST_THROW_EXCEPTION(exception::StringParseFailed<QString>(string));
        }
        return value;
    }
};

} // namespace mpk::metric_units
