#include "gqtest"

#include "TestSerialization.h"

#include "Json.h"
#include "json/JsonUtilities.h"

#include <QCryptographicHash>

#include <limits>

/*
TEST PLAN

* testPositiveInteger
* testNegativeInteger
* testUnsignedInteger
* testPositiveStdInt64
* testNegativeStdInt64
* testStdUInt64
* testPositiveQInt64
* testNegativeQInt64
* testQUInt64
* testQByteArray
* testQDateTime
* testQDate
* parseIntAsBool
* parseBooleanAsBool
* testQEnum
*/

// clang-format off
// NOLINTBEGIN(*-avoid-non-const-global-variables, *-magic-numbers, *-avoid-goto)
// clang-format on

namespace json::tests
{

struct Object
{
    Q_GADGET

public:
    enum E
    {
        One = 1,
        Two = 2
    };
    Q_ENUM(E)
};

TEST(JsonTest, testPositiveInteger)
{
    auto value = std::numeric_limits<int>::max() - 500;
    testSerialization(value);
}

TEST(JsonTest, testNegativeInteger)
{
    auto value = std::numeric_limits<int>::min() + 500;
    testSerialization(value);
}

TEST(JsonTest, testUnsignedInteger)
{
    auto value = std::numeric_limits<unsigned int>::max() - 1000;
    testSerialization(value);
}

TEST(JsonTest, testPositiveStdInt64)
{
    auto value = std::numeric_limits<std::int64_t>::max() - 500;
    testSerialization(value);
}

TEST(JsonTest, testNegativeStdInt64)
{
    auto value = std::numeric_limits<std::int64_t>::min() + 500;
    testSerialization(value);
}

TEST(JsonTest, testStdUInt64)
{
    auto value = std::numeric_limits<std::uint64_t>::max() - 1000;
    testSerialization(value);
}

TEST(JsonTest, testPositiveQInt64)
{
    auto value = std::numeric_limits<qint64>::max() - 500;
    testSerialization(value);
}

TEST(JsonTest, testNegativeQInt64)
{
    auto value = std::numeric_limits<qint64>::min() + 500;
    testSerialization(value);
}

TEST(JsonTest, testQUInt64)
{
    auto value = std::numeric_limits<quint64>::max() - 1000;
    testSerialization(value);
}

TEST(JsonTest, testQByteArray)
{
    QByteArray expected =
        QCryptographicHash::hash(QString("Hello, world!").toLocal8Bit(), QCryptographicHash::Md5);
    testSerialization(expected);
}

TEST(JsonTest, parseIntAsBool)
{
    EXPECT_FALSE(json::fromValue<bool>(QJsonValue(0)));
    EXPECT_TRUE(json::fromValue<bool>(QJsonValue(1)));
    EXPECT_TRUE(json::fromValue<bool>(QJsonValue(100)));
}

TEST(JsonTest, parseBooleanAsBool)
{
    EXPECT_FALSE(json::fromValue<bool>(QJsonValue(false)));
    EXPECT_TRUE(json::fromValue<bool>(QJsonValue(true)));
}

TEST(JsonTest, testOptional)
{
    auto parser = json::ObjectParser{QJsonObject{{"int", 1}, {"string", "str"}}};
    auto const intValue = parser.get<std::optional<int>>("int");
    auto const stringValue = parser.get<std::optional<std::string>>("string");
    auto const nulloptStringValue = parser.get<std::optional<std::string>>("notExists");

    EXPECT_EQ(1, *intValue);
    EXPECT_EQ("str", *stringValue);
    EXPECT_EQ(std::nullopt, nulloptStringValue);
}

TEST(JsonTest, testQEnum)
{
    testSerialization(Object::Two);

    auto res = json::fromValue<Object::E>(QJsonValue{2});
    EXPECT_EQ(Object::E::Two, res);
}

} // namespace json::tests

// clang-format off
// NOLINTEND(*-avoid-non-const-global-variables, *-magic-numbers, *-avoid-goto)
// clang-format on

#include "JsonParsersTests.moc"
