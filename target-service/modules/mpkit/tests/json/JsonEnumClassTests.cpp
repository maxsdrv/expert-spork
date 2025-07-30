#include "gqtest"

#include "TestSerialization.h"

#include "json/composers/EnumClass.h"
#include "json/parsers/EnumClass.h"

#include "json/JsonUtilities.h"

#include "QClassWithEnum.h"

enum class Specific
{
    ONE,
    TWO
};

template <>
struct ToJsonConverter<Specific>
{
    static QJsonValue convert(Specific value)
    {
        switch (value)
        {
            case Specific::ONE: return "one";
            case Specific::TWO: return "two";
            default: BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(""));
        }
    }
};

template <>
struct FromJsonConverter<Specific>
{
    static Specific get(const QJsonValue& value)
    {
        auto const string = json::fromValue<QString>(value);
        if (value == "one")
        {
            return Specific::ONE;
        }
        if (value == "two")
        {
            return Specific::TWO;
        }

        BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(string));
    }
};

namespace json::tests
{

TEST(JsonTest, testEnumClass)
{
    QJsonObject object{{"one", 1}, {"two", 2}, {"three", false}};

    enum class Enumeration
    {
        ONE = 1,
        TWO = 2
    };

    auto const expected = Enumeration::ONE;
    testSerialization(expected);

    EXPECT_EQ(Enumeration::TWO, json::fromObject<Enumeration>(object, "two"));
}

TEST(JsonTest, testQtEnumClass)
{
    constexpr auto tag = "enum_value";

    {
        QJsonObject expected{{tag, "TWO"}};
        QJsonObject json{{tag, json::toValue(QClassWithEnum::Enumeration::TWO)}};

        const auto expectedStr = json::toByteArray(expected).toStdString();
        const auto resultStr = json::toByteArray(json).toStdString();

        EXPECT_EQ(expectedStr, resultStr);
    }

    {
        QJsonObject object{{tag, "TWO"}};
        auto const expected = QClassWithEnum::Enumeration::TWO;
        auto const result =
            json::fromObject<QClassWithEnum::Enumeration>(object, tag);

        EXPECT_EQ(expected, result);
    }
}

TEST(JsonTest, testSpecificParserOverridesCommonForEnumClass)
{
    auto const expected = Specific::ONE;
    EXPECT_EQ(QJsonValue{"one"}, json::toValue(expected));
    testSerialization(expected);

    EXPECT_THROW(
        [[maybe_unused]] auto a = json::fromValue<Specific>(QJsonValue("three")),
        exception::json::UnexpectedValue);
}

} // namespace json::tests
