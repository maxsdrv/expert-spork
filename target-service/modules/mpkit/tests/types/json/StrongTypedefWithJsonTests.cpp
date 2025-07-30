#include "types/StrongTypedef.h"

#include "types/strong_typedef/json/JsonComposer.hpp"
#include "types/strong_typedef/json/JsonParser.hpp"
#include "types/strong_typedef/json/TemplateString.hpp"

#include "json/composers/General.h"
#include "json/parsers/General.h"

#include <gtest/gtest.h>

class IntId : public mpk::types::StrongTypedef<IntId, mpk::types::TypeIdentity<int>>,
              public mpk::types::SerializableAsJsonValue<IntId>
{
    using StrongTypedef::StrongTypedef;
};

class StringId : public mpk::types::StrongTypedef<StringId, mpk::types::TypeIdentity<QString>>,
                 public mpk::types::SerializableAsJsonValue<StringId>
{
    using StrongTypedef::StrongTypedef;
};

template <typename T>
void testSerialization(T const& expected)
{
    const auto json = json::toValue(expected);
    auto actual = json::fromValue<T>(json);
    EXPECT_EQ(expected, actual);
}

TEST(StrongTypedefTest, testJsonParsing)
{
    auto json = QJsonObject{{"int", 800}, {"string", "test"}};
    auto intId = json::fromObject<IntId>(json, "int");
    auto stringId = json::fromObject<StringId>(json, "string");

    EXPECT_EQ(intId, IntId{800});
    EXPECT_EQ(stringId, StringId{"test"});
}

TEST(StrongTypedefTest, testJsonSerializer)
{
    testSerialization(IntId{100500});
    testSerialization(StringId{"some text string"});
}

class AsObject : public mpk::types::StrongTypedef<AsObject, mpk::types::TypeIdentity<QString>>,
                 public mpk::types::SerializableAsJsonObject<AsObject, TPL_STR("some key")>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testJsonSerializerAsObjectWithCustomKey)
{
    auto const text = QString{"some text string"};
    auto const object = AsObject{text};
    testSerialization(object);

    auto const json = json::toValue(object);
    ASSERT_TRUE(json.isObject());
    EXPECT_EQ(json.toObject().value("some key").toString(), text);
}

struct CustomTag
{
    static auto value()
    {
        return QString("value");
    }
};

class AsObjectWithCustomTag
  : public mpk::types::StrongTypedef<AsObjectWithCustomTag, mpk::types::TypeIdentity<QString>>,
    public mpk::types::SerializableAsJsonObject<AsObjectWithCustomTag, CustomTag>
{
    using StrongTypedef::StrongTypedef;
};

TEST(StrongTypedefTest, testJsonSerializerAsObjectWithCustomTag)
{
    auto const text = QString{"some text string"};
    auto const object = AsObjectWithCustomTag{text};
    testSerialization(object);

    auto const json = json::toValue(object);
    ASSERT_TRUE(json.isObject());
    EXPECT_EQ(json.toObject().value("value").toString(), text);
}
