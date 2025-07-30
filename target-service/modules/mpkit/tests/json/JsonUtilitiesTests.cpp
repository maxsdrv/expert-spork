#include "gqtest"

#include "json/FromJson.h"
#include "json/JsonUtilities.h"
#include "json/composers/General.h"
#include "json/parsers/General.h"

namespace json::tests
{

TEST(JsonTest, testMerge)
{
    QJsonObject toObject{{"one", 1}, {"two", "two"}, {"three", false}};
    QJsonObject fromObject{{"four", "four"}, {"three", true}};

    json::merge(toObject, fromObject);
    EXPECT_EQ(1, json::fromObject<int>(toObject, "one"));
    EXPECT_EQ("two", json::fromObject<QString>(toObject, "two"));
    EXPECT_EQ(true, json::fromObject<bool>(toObject, "three"));
    EXPECT_EQ("four", json::fromObject<QString>(toObject, "four"));
}

TEST(JsonTest, testPutUniqueValue)
{
    QJsonObject object;
    QString key("someKey");
    json::putUniqueValue(object, key, 100);
    EXPECT_THROW(json::putUniqueValue(object, key, true), exception::json::DuplicateField);
}

TEST(JsonTest, testToStringWithDefaultFormat)
{
    const QJsonObject json{{"one", 1}, {"two", "two"}};
    const std::string expectedString =
        R"({
    "one": 1,
    "two": "two"
}
)";

    const std::string resultString = json::toByteArray(json).toStdString();

    EXPECT_EQ(resultString, expectedString);
}

TEST(JsonTest, testToStringWithCompactFormat)
{
    const QJsonObject json{{"one", 1}, {"two", "two"}};
    const std::string expectedString = R"({"one":1,"two":"two"})";

    const std::string resultString =
        json::toByteArray(json, QJsonDocument::Compact).toStdString();

    EXPECT_EQ(resultString, expectedString);
}

TEST(JsonTest, testFromByteArray)
{
    const QJsonObject expected{{"one", 1}, {"two", "two"}};
    const auto viaCompact =
        json::fromByteArray(json::toByteArray(expected, QJsonDocument::Compact));
    const auto viaIndented =
        json::fromByteArray(json::toByteArray(expected, QJsonDocument::Indented));

    EXPECT_EQ(viaCompact, expected);
    EXPECT_EQ(viaIndented, expected);
}

TEST(JsonTest, testReplaceSimpleValuePartialMatch)
{
    auto object =
        QJsonObject{{"one", 1}, {"password", "replace"}, {"not_password", "not_replace"}};

    object = json::replaceValue(object, QRegularExpression{"^password$"}, "replaced");
    EXPECT_EQ(1, object.value("one").toInt());
    EXPECT_EQ("replaced", object.value("password").toString());
    EXPECT_EQ("not_replace", object.value("not_password").toString());
}

TEST(JsonTest, testReplaceSimpleValueFullMatch)
{
    auto object =
        QJsonObject{{"one", 1}, {"password", "replace"}, {"not_password", "replace_too"}};
    object = json::replaceValue(object, QRegularExpression{".*password.*"}, "replaced");

    EXPECT_EQ(1, object.value("one").toInt());
    EXPECT_EQ("replaced", object.value("password").toString());
    EXPECT_EQ("replaced", object.value("not_password").toString());
}

TEST(JsonTest, testReplaceNestedValue)
{
    auto object = QJsonObject{
        {"one", 1},
        {"nested", QJsonObject{{"password", "replace"}}},
        {"not_password", "replace_too"}};
    object = json::replaceValue(object, QRegularExpression{".*password.*"}, "replaced");

    EXPECT_EQ("replaced", object.value("nested").toObject().value("password").toString());
    EXPECT_EQ("replaced", object.value("not_password").toString());
}

TEST(JsonTest, testReplaceSubtree)
{
    auto object = QJsonObject{{{"nested", QJsonObject{{"password", "password"}}}}};
    object = json::replaceValue(object, QRegularExpression{".*nested.*"}, "replaced");

    EXPECT_EQ("replaced", object.value("nested").toString());
}

TEST(JsonTest, testReplaceArrayValue)
{
    auto object = QJsonObject{
        {"one", 1},
        {"array", QJsonArray{1, QJsonObject{{"password", "password"}}}},
        {"not_password", "not_password"}};
    object = json::replaceValue(object, QRegularExpression{".*password.*"}, "replaced");

    EXPECT_EQ(
        "replaced",
        object.value("array").toArray().at(1).toObject().value("password").toString());
    EXPECT_EQ("replaced", object.value("not_password").toString());
}

} // namespace json::tests
