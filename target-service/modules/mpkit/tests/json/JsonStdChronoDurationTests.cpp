#if (__cpp_lib_chrono >= 201907L && __cpp_lib_format >= 201907L) || __has_include("date/date.h")

#include "gqtest"

#include "TestSerialization.h"

#include "json/composers/General.h"
#include "json/composers/StdChronoDuration.h"
#include "json/parsers/StdChronoDuration.h"

/*

TEST PLAN:

1. Units
    1.1.  unitNanoseconds
    1.2.  unitMicroseconds
    1.3.  unitMilliseconds
    1.4.  unitSeconds
    1.5.  unitMinutes
    1.6.  unitHours
    1.7.  unitDays
    1.8.  unitWeeks
    1.9.  unitMonths
    1.10. unitYears
2. Intervals
    2.1. intervalYears
3. Optionals
    3.1. optionalMilliseconds
    3.2. optionalEmpty

 */

namespace json::tests
{

using namespace std::chrono;
using namespace std::chrono_literals;

TEST(JsonStdChronoDurationTest, unitNanoseconds)
{
    const auto expected = 1337ns;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "00:00:00.000001337");

    EXPECT_EQ(json::fromValue<nanoseconds>("00:00:00.000001337"), expected);
    EXPECT_EQ(json::fromValue<nanoseconds>("00:00:01.337133713"), 1'337'133'713ns);
}

TEST(JsonStdChronoDurationTest, unitMicroseconds)
{
    const auto expected = 1337us;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "00:00:00.001337");

    EXPECT_EQ(json::fromValue<microseconds>("0:0:0.001337"), expected);
    EXPECT_EQ(json::fromValue<nanoseconds>("00:00:00.001337"), expected);
}

TEST(JsonStdChronoDurationTest, unitMilliseconds)
{
    const auto expected = 1337ms;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "00:00:01.337");

    EXPECT_EQ(json::fromValue<milliseconds>("00:00:01.337"), expected);
    EXPECT_EQ(json::fromValue<milliseconds>("00:22:17.133"), 1'337'133ms);
}

TEST(JsonStdChronoDurationTest, unitSeconds)
{
    const auto expected = 1337s;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "00:22:17");
    EXPECT_EQ(json::toValueWithFormat(expected, "%M:%S").toString(), "22:17");
    EXPECT_EQ(json::toValueWithFormat(expected, "%S").toString(), "17");

#if __cpp_lib_format >= 201907L
    EXPECT_THROW(json::toValueWithFormat(expected, "%BAD").toString(), std::format_error);
#endif

    EXPECT_EQ(json::fromValue<seconds>("00:22:17"), expected);
    EXPECT_EQ(json::fromValueWithFormat<seconds>("22:17", "%M:%S"), expected);
    EXPECT_EQ(json::fromValueWithFormat<seconds>("1337", "%0S"), expected);
    EXPECT_EQ(json::fromValueWithFormat<seconds>("17", "%S"), 17s);

    EXPECT_THROW(
        [[maybe_unused]] auto value = json::fromValueWithFormat<seconds>("17", "%BAD"),
        exception::json::UnexpectedValue);
    EXPECT_THROW(
        [[maybe_unused]] auto value = json::fromValueWithFormat<seconds>("BAD", "%S"),
        exception::json::UnexpectedValue);
    EXPECT_THROW(
        [[maybe_unused]] auto value = json::fromValueWithFormat<seconds>("BAD", "%BAD"),
        exception::json::UnexpectedValue);
}

TEST(JsonStdChronoDurationTest, unitMinutes)
{
    const auto expected = 1337min;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "22:17:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%H:%M:%S").toString(), "22:17:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%H:%M").toString(), "22:17");
    EXPECT_EQ(json::toValueWithFormat(expected, "%M").toString(), "17");
    EXPECT_EQ(json::toValueWithFormat(expected, "%T").toString(), "22:17:00");

    EXPECT_EQ(json::fromValue<minutes>("22:17:00"), expected);
    EXPECT_EQ(json::fromValueWithFormat<minutes>("22:17:00", "%H:%M:%S"), expected);
    EXPECT_EQ(json::fromValueWithFormat<minutes>("22:17", "%H:%M"), expected);
    EXPECT_EQ(json::fromValueWithFormat<minutes>("1337", "%0M"), expected);
    EXPECT_EQ(json::fromValueWithFormat<minutes>("17", "%M"), 17min);
}

TEST(JsonStdChronoDurationTest, unitHours)
{
    const auto expected = 1337h;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "1337:00:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%T").toString(), "1337:00:00");
#if __cplusplus < 202002L
    EXPECT_EQ(json::toValueWithFormat(expected, "%H:%M:%S").toString(), "1337:00:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%H").toString(), "1337");
#else
    // Beware, results is not the same in C++20 and looks like broken
    // EXPECT_EQ(json::toValueWithFormat(expected, "%H:%M:%S").toString(), "1337:00:00");
    // EXPECT_EQ(json::toValueWithFormat(expected, "%H").toString(), "1337");
#endif

    EXPECT_EQ(json::fromValue<hours>("1337:00:00"), expected);
    EXPECT_EQ(json::fromValueWithFormat<hours>("1337:00:00", "%0H:%M:%S"), expected);
    EXPECT_EQ(json::fromValueWithFormat<hours>("1337:59:59", "%0H"), expected);
    EXPECT_EQ(json::fromValueWithFormat<hours>("1337", "%0H"), expected);
    EXPECT_EQ(json::fromValueWithFormat<hours>("17", "%H"), 17h);
}

TEST(JsonStdChronoDurationTest, unitDays)
{
#if __cplusplus < 202002L
    using namespace date;
#endif

    const auto expected = days{1337};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "32088:00:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%j").toString(), "1337");

    EXPECT_EQ(json::fromValue<days>("32088:00:00"), expected);
    EXPECT_EQ(json::fromValueWithFormat<days>("1337", "%0j"), expected);
}

TEST(JsonStdChronoDurationTest, unitWeeks)
{
#if __cplusplus < 202002L
    using namespace date;
#endif

    const auto expected = weeks{1337};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "224616:00:00");
    EXPECT_EQ(json::toValueWithFormat(expected, "%j").toString(), "9359");

    EXPECT_EQ(json::fromValue<weeks>("224616:00:00"), expected);
    EXPECT_EQ(json::fromValueWithFormat<weeks>("9359", "%0j"), expected);
}

TEST(JsonStdChronoDurationTest, unitMonths)
{
#if __cplusplus < 202002L
    using namespace date;
#endif

    const auto expected = months{1337};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "976658:26:42");

    EXPECT_EQ(json::fromValue<months>("976658:26:42"), expected);
}

TEST(JsonStdChronoDurationTest, unitYears)
{
#if __cplusplus < 202002L
    using namespace date;
#endif

    const auto expected = years{1337};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "11719901:20:24");

    EXPECT_EQ(json::fromValue<years>("11719901:20:24"), expected);
}

TEST(JsonStdChronoDurationTest, intervalYears)
{
#if __cplusplus < 202002L
    using namespace date;
#endif

    constexpr system_clock::time_point begin =
        sys_days{May / 26 / 1799} + 12h + 35min + 34s + 345ms + 653us + 723ns;
    constexpr system_clock::time_point end =
        sys_days{January / 29 / 1837} + 9h + 17min + 12s + 82ms + 997us + 1ns;

    const auto expected = end - begin;
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "330284:41:37.737343278");

    EXPECT_EQ(json::fromValue<nanoseconds>("330284:41:37.737343278"), expected);
    EXPECT_EQ(
        json::fromValueWithFormat<nanoseconds>("330284:41:37.737343278", "%0H:%M:%S"), expected);
}

TEST(JsonStdChronoDurationTest, optionalMilliseconds)
{
    using MillisecondsOptional = std::optional<milliseconds>;

    const auto expected = MillisecondsOptional{1337ms};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected).toString(), "00:00:01.337");
    EXPECT_EQ(json::toValueWithFormat(expected, "%S").toString(), "01.337");

#if __cpp_lib_format >= 201907L
    EXPECT_THROW(json::toValueWithFormat(expected, "%BAD").toString(), std::format_error);
#endif

    EXPECT_EQ(json::fromValue<MillisecondsOptional>("00:00:01.337"), expected);
    EXPECT_EQ(json::fromValue<MillisecondsOptional>("00:22:17.133"), 1'337'133ms);
    EXPECT_EQ(json::fromValueWithFormat<MillisecondsOptional>("01.337", "%S"), expected);
    EXPECT_EQ(json::fromValueWithFormat<MillisecondsOptional>("22:17.133", "%M:%S"), 1'337'133ms);

    EXPECT_THROW(
        [[maybe_unused]] auto value =
            json::fromValueWithFormat<MillisecondsOptional>("01.337", "%BAD"),
        exception::json::UnexpectedValue);
    EXPECT_THROW(
        [[maybe_unused]] auto value =
            json::fromValueWithFormat<MillisecondsOptional>("BAD", "%S"),
        exception::json::UnexpectedValue);
    EXPECT_THROW(
        [[maybe_unused]] auto value =
            json::fromValueWithFormat<MillisecondsOptional>("BAD", "%BAD"),
        exception::json::UnexpectedValue);
}

TEST(JsonStdChronoDurationTest, optionalEmpty)
{
    using MillisecondsOptional = std::optional<milliseconds>;

    const auto expected = MillisecondsOptional{std::nullopt};
    testSerialization(expected);

    EXPECT_EQ(json::toValue(expected), QJsonValue{});

    EXPECT_EQ(json::fromValue<MillisecondsOptional>(QJsonValue{}), expected);
}

} // namespace json::tests

#endif // #if (__cpp_lib_chrono >= 201907L && __cpp_lib_format >= 201907L) ||
       // __has_include("date/date.h")
