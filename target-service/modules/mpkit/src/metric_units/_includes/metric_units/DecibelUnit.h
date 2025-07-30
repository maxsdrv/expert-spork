#pragma once

#include "exception/StringParseFailed.h"

#include "metric_units/Decibel.h"
#include "metric_units/Helpers.h"

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/JsonComposer.hpp"
#include "types/strong_typedef/json/JsonParser.hpp"
#include "types/strong_typedef/json/Traits.hpp"

#include <QCoreApplication>

namespace mpk::metric_units
{

/*
struct SignalLevelTraits
{
    using Type = double;
    static constexpr auto name = "uV";
}
*/

struct DecibelUnitTag
{
    static constexpr auto value() noexcept
    {
        return "value";
    }
};

template <typename Traits>
class DecibelUnit : public types::StrongTypedef<DecibelUnit<Traits>, Traits>,
                    public types::Ordered<DecibelUnit<Traits>>,
                    public types::Additive<DecibelUnit<Traits>, Decibel>,
                    public types::SerializableAsJsonObject<DecibelUnit<Traits>, DecibelUnitTag>

{
public:
    using StrongType = types::StrongTypedef<DecibelUnit<Traits>, Traits>;
    using ValueType = typename Traits::ValueType;
    using StrongType::value;
    using types::StrongTypedef<DecibelUnit<Traits>, Traits>::StrongTypedef;

    static QString unitString()
    {
        return unitStringHelper<helpers::DisplayName>();
    }

    template <bool printUnits = true>
    QString toString(int precision = 2, bool omitTrailingZeros = false) const
    {
        return toStringHelper<helpers::Name, printUnits>(precision, omitTrailingZeros);
    }

    template <bool printUnits = true>
    QString toDisplayString(int precision = 2, bool omitTrailingZeros = false) const
    {
        return toStringHelper<helpers::DisplayName, printUnits>(precision, omitTrailingZeros);
    }

    template <bool printUnits = true>
    QString toNormalizedString(int precision = 2, bool omitTrailingZeros = false) const
    {
        return toDisplayString<printUnits>(precision, omitTrailingZeros);
    }

    constexpr static DecibelUnit fromString(const QString& string)
    {
        return fromStringHelper<helpers::Name>(string);
    }

    constexpr static DecibelUnit fromDisplayString(const QString& string)
    {
        return fromStringHelper<helpers::DisplayName>(string);
    }

private:
    template <template <typename> typename N>
    static QString unitStringHelper()
    {
        return N<DecibelTraits>::value() + N<Traits>::value();
    }

    template <template <typename> typename N, bool printUnits>
    QString toStringHelper(int precision, bool omitTrailingZeros) const noexcept
    {
        auto val = static_cast<double>(value());
        auto str = QString::number(val, 'f', precision);
        if (precision > 0 && omitTrailingZeros)
        {
            str = helpers::chopTrailingZeros(str);
        }
        if constexpr (printUnits)
        {
            return str + " " + unitStringHelper<N>();
        }
        return str;
    }

    template <template <typename> typename N>
    static DecibelUnit fromStringHelper(const QString& string)
    {
        int index = 0;
        while (index < string.size() && (*Traits::numericValidate)(string.at(index)))
        {
            index++;
        }

        auto numPart = string.left(index);
        auto unitPart = string.mid(index).trimmed();
        if (!unitPart.isEmpty() && unitPart != unitStringHelper<N>())
        {
            BOOST_THROW_EXCEPTION(exception::StringParseFailed<QString>(string));
        }

        auto ok = false;
        auto value = numPart.toDouble(&ok);
        if (!ok)
        {
            BOOST_THROW_EXCEPTION(exception::StringParseFailed<QString>(string));
        }

        return DecibelUnit{static_cast<ValueType>(value)};
    }
};

} // namespace mpk::metric_units
