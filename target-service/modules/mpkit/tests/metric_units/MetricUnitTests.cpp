#include "metric_units/MetricUnits.h"

#include <gmock/gmock.h>

using namespace mpk::metric_units;
using namespace mpk::metric_units::prefix;
using namespace mpk::metric_units::numeric_validator;

struct FrequencyAllPrefixesTraits
{
    using ValueType = std::uint64_t;
    using Prefix = prefix::None;
    using PrefixList = prefix::ListAll;
    static constexpr auto name = "Hz";
    static constexpr auto displayName = QT_TR_NOOP("Hz");
    static inline const auto numericValidate = &numeric_validator::pointOnly;
    static constexpr bool implicitConstructor = true;
};

using Frequency = MetricUnit<FrequencyAllPrefixesTraits>;

struct FrequencyStandardPrefixesTraits
{
    using ValueType = std::uint64_t;
    using Prefix = prefix::None;
    using PrefixList = prefix::ListStandard;
    static constexpr auto name = "Hz";
    static constexpr auto displayName = QT_TR_NOOP("Hz");
    static inline const auto numericValidate = &numeric_validator::pointOnly;
    static constexpr bool implicitConstructor = true;
};

using FrequencyStandardPrefix = MetricUnit<FrequencyStandardPrefixesTraits>;

struct NegativeValuesTraits
{
    using ValueType = std::int64_t;
    using Prefix = prefix::None;
    using PrefixList = prefix::ListAll;
    static constexpr auto name = "Hz";
    static constexpr auto displayName = QT_TR_NOOP("Hz");
    static inline const auto numericValidate = &numeric_validator::allDouble;
    static constexpr bool implicitConstructor = true;
};

using NegativeValues = MetricUnit<NegativeValuesTraits>;

TEST(MetricUnitTests, defaultCtor)
{
    Frequency freq;
    EXPECT_EQ(0, static_cast<Frequency::ValueType>(freq));
}

TEST(MetricUnitTests, storeMeGood)
{
    auto value = Frequency::ValueType{12345};
    auto freq = Frequency(value);
    EXPECT_EQ(value, static_cast<Frequency::ValueType>(freq));
}

TEST(MetricUnitTests, operatorPlus)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{23456};
    auto freq2 = Frequency(value2);
    EXPECT_EQ(value1 + value2, static_cast<Frequency::ValueType>(freq1 + freq2));
}

TEST(MetricUnitTests, operatorPlusAssign)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{23456};
    auto freq2 = Frequency(value2);
    freq2 += freq1;
    EXPECT_EQ(value1 + value2, freq2.value());
}

TEST(MetricUnitTests, operatorMinus)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3456};
    auto freq2 = Frequency(value2);
    EXPECT_EQ(value1 - value2, static_cast<Frequency::ValueType>(freq1 - freq2));
    EXPECT_EQ(value2 - value1, static_cast<Frequency::ValueType>(freq2 - freq1));
}

TEST(MetricUnitTests, operatorMinusAssign)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3456};
    auto freq2 = Frequency(value2);
    freq1 -= freq2;
    EXPECT_EQ(value1 - value2, freq1.value());
}

TEST(MetricUnitTests, operatorMinusAssign2)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3456};
    auto freq2 = Frequency(value2);
    freq2 -= freq1;
    EXPECT_EQ(value2 - value1, freq2.value());
}

TEST(MetricUnitTests, operatorMultiply)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3};
    EXPECT_EQ(value1 * value2, static_cast<Frequency::ValueType>(freq1 * value2));
}

TEST(MetricUnitTests, operatorMultiplyFloat)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = 3.5F;
    EXPECT_EQ(static_cast<Frequency::ValueType>(value1 * value2),
              static_cast<Frequency::ValueType>(freq1 * value2));
}

TEST(MetricUnitTests, operatorMultiplyAssign)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3};
    freq1 *= value2;
    EXPECT_EQ(value1 * value2, freq1.value());
}

TEST(MetricUnitTests, operatorMultiplyAssignFloat)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = 4.5F;
    freq1 *= value2;
    EXPECT_EQ(static_cast<Frequency::ValueType>(value1 * value2), freq1.value());
}

TEST(MetricUnitTests, operatorDivide)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3};
    EXPECT_EQ(value1 / value2, static_cast<Frequency::ValueType>(freq1 / value2));
}

TEST(MetricUnitTests, operatorDivideFloat)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = 8.F;
    EXPECT_EQ(static_cast<Frequency::ValueType>(value1 / value2),
              static_cast<Frequency::ValueType>(freq1 / value2));
}

TEST(MetricUnitTests, operatorDivideAssign)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{3};
    freq1 /= value2;
    EXPECT_EQ(value1 / value2, freq1.value());
}

TEST(MetricUnitTests, operatorDivideAssignFloat)
{
    auto value1 = Frequency::ValueType{12345};
    auto freq1 = Frequency(value1);
    auto value2 = 8.63F;
    freq1 /= value2;
    EXPECT_EQ(static_cast<Frequency::ValueType>(value1 / value2), freq1.value());
}

TEST(MetricUnitTests, operatorEqual)
{
    EXPECT_EQ(Frequency(12345), Frequency(12345));
}

TEST(MetricUnitTests, operatorNotEqual)
{
    EXPECT_NE(Frequency(12345), Frequency(23456));
}

TEST(MetricUnitTests, operatorGreater)
{
    EXPECT_GT(Frequency(23456), Frequency(12345));
}

TEST(MetricUnitTests, operatorGreaterEqual)
{
    EXPECT_GE(Frequency(23456), Frequency(12345));
    EXPECT_GE(Frequency(23456), Frequency(23456));
}

TEST(MetricUnitTests, operatorLess)
{
    EXPECT_LT(Frequency(12345), Frequency(23456));
}

TEST(MetricUnitTests, operatorLessEqual)
{
    EXPECT_LE(Frequency(12345), Frequency(23456));
    EXPECT_LE(Frequency(23456), Frequency(23456));
}

TEST(MetricUnitTests, ratio)
{
    auto value1 = Frequency::ValueType{1250000};
    auto freq1 = Frequency(value1);
    auto value2 = Frequency::ValueType{150000};
    auto freq2 = Frequency(value2);
    EXPECT_DOUBLE_EQ(static_cast<double>(value1) / static_cast<double>(value2),
                     freq1 / freq2);
}

TEST(MetricUnitTests, asUp)
{
    auto value1 = Frequency::ValueType{1250000};
    auto freq1 = Frequency(value1);
    auto value2 = freq1.as<prefix::Milli>();
    EXPECT_DOUBLE_EQ(value2, 1'250'000'000.);
}

TEST(MetricUnitTests, asDown)
{
    auto value1 = Frequency::ValueType{1250000};
    auto freq1 = Frequency(value1);
    auto value2 = freq1.as<prefix::Mega>();
    EXPECT_DOUBLE_EQ(value2, 1.250);
}

TEST(MetricUnitTests, asDownRounding)
{
    auto value1 = Frequency::ValueType{1250000};
    auto freq1 = Frequency(value1);
    auto value2 = freq1.as<prefix::Mega, Frequency::ValueType>();
    EXPECT_EQ(value2, 1);
}

// PD - default precision
// P4(1,2..,N) - non-default precision
// OZ - omit trailing zeros
// PU - print units
// Base - target prefix match traits prefix
// Up - target prefix greater than traits prefix
// Down - target prefix less than traits prefix
TEST(MetricUnitTests, toStringPDBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<Frequency::ValuePrefix, false>();
    EXPECT_EQ(QString{"12345.00"}, str);
}

TEST(MetricUnitTests, toStringP0Base)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<Frequency::ValuePrefix, false>(0);
    EXPECT_EQ(QString{"12345"}, str);
}

TEST(MetricUnitTests, toStringP4Base)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<Frequency::ValuePrefix, false>(4);
    EXPECT_EQ(QString{"12345.0000"}, str);
}

TEST(MetricUnitTests, toStringP2OZBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<Frequency::ValuePrefix, false>(2, true);
    EXPECT_EQ(QString{"12345"}, str);
}

TEST(MetricUnitTests, toStringPDPUBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString();
    EXPECT_EQ(QString{"12345.00 Hz"}, str);
}

TEST(MetricUnitTests, toStringP0PUBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString(0);
    EXPECT_EQ(QString{"12345 Hz"}, str);
}

TEST(MetricUnitTests, toStringP4PUBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString(4);
    EXPECT_EQ(QString{"12345.0000 Hz"}, str);
}

TEST(MetricUnitTests, toStringP2OZPUBase)
{
    auto freq = Frequency{12345};
    auto str = freq.toString(2, true);
    EXPECT_EQ(QString{"12345 Hz"}, str);
}

TEST(MetricUnitTests, toStringPDUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Kilo, false>();
    EXPECT_EQ(QString{"12.35"}, str);
}

TEST(MetricUnitTests, toStringP0Up)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Kilo, false>(0);
    EXPECT_EQ(QString{"12"}, str);
}

TEST(MetricUnitTests, toStringP4Up)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Kilo, false>(4);
    EXPECT_EQ(QString{"12.3450"}, str);
}

TEST(MetricUnitTests, toStringP4OZUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Kilo, false>(4, true);
    EXPECT_EQ(QString{"12.345"}, str);
}

TEST(MetricUnitTests, toStringPDPUUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Mega>();
    EXPECT_EQ(QString{"0.01 MHz"}, str);
}

TEST(MetricUnitTests, toStringP0PUUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Mega>(0);
    EXPECT_EQ(QString{"0 MHz"}, str);
}

TEST(MetricUnitTests, toStringP4PUUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Mega>(4);
    EXPECT_EQ(QString{"0.0123 MHz"}, str);
}

TEST(MetricUnitTests, toStringP4OZPUUp)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Mega>(4, true);
    EXPECT_EQ(QString{"0.0123 MHz"}, str);
}

TEST(MetricUnitTests, toStringPDDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Deci, false>();
    EXPECT_EQ(QString{"123450.00"}, str);
}

TEST(MetricUnitTests, toStringP0Down)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Deci, false>(0);
    EXPECT_EQ(QString{"123450"}, str);
}

TEST(MetricUnitTests, toStringP4Down)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Deci, false>(4);
    EXPECT_EQ(QString{"123450.0000"}, str);
}

TEST(MetricUnitTests, toStringP2OZDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Deci, false>(2, true);
    EXPECT_EQ(QString{"123450"}, str);
}

TEST(MetricUnitTests, toStringPDPUDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Milli>();
    EXPECT_EQ(QString{"12345000.00 mHz"}, str);
}

TEST(MetricUnitTests, toStringP0PUDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Milli>(0);
    EXPECT_EQ(QString{"12345000 mHz"}, str);
}

TEST(MetricUnitTests, toStringP4PUDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Milli>(4);
    EXPECT_EQ(QString{"12345000.0000 mHz"}, str);
}

TEST(MetricUnitTests, toStringP2OZPUDown)
{
    auto freq = Frequency{12345};
    auto str = freq.toString<prefix::Milli>(2, true);
    EXPECT_EQ(QString{"12345000 mHz"}, str);
}

TEST(MetricUnitTests, toNormalizedStringPD)
{
    auto freq = Frequency{12345};
    auto str = freq.toNormalizedString<false>();
    EXPECT_EQ(QString{"12.35"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP0)
{
    auto freq = Frequency{12345};
    auto str = freq.toNormalizedString<false>(0);
    EXPECT_EQ(QString{"12"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP4)
{
    auto freq = Frequency{12345};
    auto str = freq.toNormalizedString<false>(4);
    EXPECT_EQ(QString{"12.3450"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP4OZ)
{
    auto freq = Frequency{12345};
    auto str = freq.toNormalizedString<false>(4, true);
    EXPECT_EQ(QString{"12.345"}, str);
}

TEST(MetricUnitTests, toNormalizedStringPDPU)
{
    auto freq = Frequency{12345678};
    auto str = freq.toNormalizedString();
    EXPECT_EQ(QString{"12.35 MHz"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP0PU)
{
    auto freq = Frequency{12345678};
    auto str = freq.toNormalizedString(0);
    EXPECT_EQ(QString{"12 MHz"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP4PU)
{
    auto freq = Frequency{12345678};
    auto str = freq.toNormalizedString(4);
    EXPECT_EQ(QString{"12.3457 MHz"}, str);
}

TEST(MetricUnitTests, toNormalizedStringP2OZPU)
{
    auto freq = Frequency{12345678};
    auto str = freq.toNormalizedString(2, true);
    EXPECT_EQ(QString{"12.35 MHz"}, str);
}

TEST(MetricUnitTests, fromStringNoUnit)
{
    auto freq = Frequency::fromString<prefix::Kilo>("12345");
    EXPECT_EQ(12345000UL, static_cast<Frequency::ValueType>(freq));
    freq = Frequency::fromString<prefix::Micro>("12345789");
    EXPECT_EQ(12, static_cast<Frequency::ValueType>(freq));
}

TEST(MetricUnitTests, fromStringUnit)
{
    auto freq = Frequency::fromString("12345 kHz");
    EXPECT_EQ(12345000, static_cast<Frequency::ValueType>(freq));
    freq = Frequency::fromString("12345789uHz");
    EXPECT_EQ(12, static_cast<Frequency::ValueType>(freq));
    freq = Frequency::fromString("123     MHz");
    EXPECT_EQ(123000000, static_cast<Frequency::ValueType>(freq));

    auto freq2 = FrequencyStandardPrefix::fromString("123 MHz");
    EXPECT_EQ(123000000, static_cast<FrequencyStandardPrefix::ValueType>(freq2));

    auto freq3 = NegativeValues::fromString("-12345");
    EXPECT_EQ(NegativeValues(-12345), freq3);
    freq3 = NegativeValues::fromString("+12345");
    EXPECT_EQ(NegativeValues(12345), freq3);
    freq3 = NegativeValues::fromString("1e2");
    EXPECT_EQ(NegativeValues(100), freq3);
}

TEST(MetricUnitTests, fromStringNoUnitBad)
{
    EXPECT_THROW(Frequency::fromString<prefix::Kilo>("hz12345"), std::exception);
    EXPECT_THROW(Frequency::fromString<prefix::Kilo>("zdas12432--kHz"), std::exception);

    // Not supported parse symbols
    EXPECT_THROW(Frequency::fromString<prefix::Kilo>("-12345"), std::exception);
    EXPECT_THROW(Frequency::fromString<prefix::Kilo>("+12345"), std::exception);
    EXPECT_THROW(Frequency::fromString<prefix::Kilo>("1e5"), std::exception);
}

TEST(MetricUnitTests, fromStringUnitBad)
{
    EXPECT_THROW(Frequency::fromString("12345 mkHz"), std::exception);
    EXPECT_THROW(Frequency::fromString("12345 kTr"), std::exception);
    EXPECT_THROW(Frequency::fromString("12345--kHz"), std::exception);

    // Not supported parse symbols
    EXPECT_THROW(Frequency::fromString("1234,5 Hz"), std::exception);
    EXPECT_THROW(Frequency::fromString("-12345 Hz"), std::exception);
    EXPECT_THROW(Frequency::fromString("+12345 Hz"), std::exception);
    EXPECT_THROW(Frequency::fromString("1e2 Hz"), std::exception);

    EXPECT_THROW(FrequencyStandardPrefix::fromString("123 hHz"), std::exception);
}

TEST(MetricUnitTests, fromValueUp)
{
    EXPECT_EQ(Frequency{12345}, Frequency::fromValue<prefix::Kilo>(12.345));
    EXPECT_EQ(Frequency{12345000}, Frequency::fromValue<prefix::Kilo>(12345));
}

TEST(MetricUnitTests, fromValueSame)
{
    EXPECT_EQ(Frequency{12345}, Frequency::fromValue<prefix::None>(12345));
    EXPECT_EQ(Frequency{12345}, Frequency::fromValue(12345));
}

TEST(MetricUnitTests, fromValueDown)
{
    EXPECT_EQ(Frequency{12345}, Frequency::fromValue<prefix::Milli>(12345000));
    EXPECT_EQ(Frequency{12}, Frequency::fromValue<prefix::Milli>(12345));
}

TEST(MetricUnitTests, fromValueDownRounding)
{
    EXPECT_EQ(Frequency{12}, Frequency::fromValue<prefix::Milli>(12345));
}
