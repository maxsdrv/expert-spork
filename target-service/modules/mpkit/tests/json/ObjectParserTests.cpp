#include "gqtest"

#include "json/ObjectParser.h"
#include "json/parsers/General.h"
#include "json/parsers/QJsonTypes.h"

// Test suite to test object parser
namespace json::tests
{

TEST(JsonTest, testObjectParser)
{
    QJsonValue value = QJsonObject{{"one", 1}, {"bool", false}, {"string", "str"}};
    const json::ObjectParser parser{value};
    EXPECT_EQ(1, parser.get<int>("one"));
    EXPECT_FALSE(parser.get<bool>("bool", true));
    EXPECT_EQ("str", parser.get<QString>("string"));
    EXPECT_EQ("default", parser.get<QString>("notExisted", "default"));
}

TEST(JsonTest, testParseToWithObjectParser)
{
    QJsonValue value = QJsonObject{{"int", 1}, {"bool", false}, {"string", "str"}};
    int intValue;
    bool boolValue;
    QString stringValue;
    QString stringValueWithDefault;

    json::ObjectParser{value}
        .to(intValue, "int")
        .to(boolValue, "bool", true)
        .to(stringValue, "string")
        .to(stringValueWithDefault, "notExisted", QString{"default"});

    EXPECT_EQ(1, intValue);
    EXPECT_FALSE(boolValue);
    EXPECT_EQ("str", stringValue);
    EXPECT_EQ("default", stringValueWithDefault);
}

TEST(JsonTest, testObjectParserWithIterations)
{
    auto array = QJsonArray{QJsonObject{{"value", 0}}, QJsonObject{{"value", 1}}};
    int expected{0};
    for (const json::ObjectParser parser: array)
    {
        EXPECT_EQ(expected++, parser.get<int>("value"));
    }
}

} // namespace json::tests
