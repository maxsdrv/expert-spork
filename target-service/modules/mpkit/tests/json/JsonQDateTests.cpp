#include "gqtest"

#include "TestSerialization.h"

#include "json/composers/QDate.h"
#include "json/parsers/QDate.h"

namespace json::tests
{

TEST(JsonTest, testQDateTime)
{
    auto expected = QDateTime::fromString("2019-11-21T12:22:48", Qt::ISODate);
    testSerialization(expected);
}

TEST(JsonTest, testQDate)
{
    auto expected = QDate::fromString("2019-11-21", Qt::ISODate);
    testSerialization(expected);
}

TEST(JsonTest, testDateWithFormat)
{
    const auto* dateStr = "2021-11-01T16:05:08.123Z";
    const auto format = Qt::ISODateWithMs;
    const auto originDate = QDateTime::fromString(dateStr, format);

    const auto json = json::toValueWithFormat(originDate, format);
    auto actualDate = json::fromValueWithFormat<QDateTime>(json, format);
    EXPECT_EQ(originDate, actualDate);
}

TEST(JsonTest, testDateWithFormatAndDefaultValue)
{
    const auto dateStr = "2021-11-01T16:05:08.123Z";
    const auto format = Qt::ISODateWithMs;

    const auto originDate = QDateTime::fromString(dateStr, format);
    const auto defaultDate = originDate.addDays(1);

    constexpr auto existTag = "date";
    constexpr auto notExistTag = "_date";

    const QJsonObject json{{existTag, dateStr}};

    const auto existDate = json::fromObjectWithFormat<QDateTime>(json, existTag, format);

    const auto notExistDate =
        json::fromObjectWithFormat<QDateTime>(json, notExistTag, format, defaultDate);

    EXPECT_EQ(originDate, existDate);
    EXPECT_EQ(notExistDate, defaultDate);
}

} // namespace json::tests
