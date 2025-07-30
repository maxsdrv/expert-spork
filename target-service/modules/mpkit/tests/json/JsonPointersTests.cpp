#include "gqtest"

#include "json/ToJson.h"
#include "json/composers/Containers.h"
#include "json/composers/General.h"
#include "json/composers/Pointers.h"
#include "json/composers/QDate.h"

#include "json/JsonUtilities.h"

#include <QJsonArray>

/* TEST PLAN

* pointer
* pointer with format
* vector with pointers
* vector with nullptr

*/

namespace json::tests
{

struct TestStruct
{
    std::string name;
    std::string value;
};

} // namespace json::tests

template <>
struct ToJsonConverter<json::tests::TestStruct>
{
    static QJsonValue convert(const json::tests::TestStruct value)
    {
        return QJsonObject{
            {"name", json::toValue(value.name)},
            {"value", json::toValue(value.value)},
        };
    }
};

namespace json::tests
{

TEST(JsonPointersTest, pointer)
{
    TestStruct object{.name = "testName", .value = "testValue"};
    const TestStruct* pointer{&object};

    QJsonObject expected{
        {
            "object",
            QJsonObject{
                {"name", "testName"},
                {"value", "testValue"},
            },
        },
    };
    QJsonObject json{{"object", json::toValue(pointer)}};

    const auto expectedStr = json::toByteArray(expected).toStdString();
    const auto resultStr = json::toByteArray(json).toStdString();

    EXPECT_EQ(expectedStr, resultStr);
}

TEST(JsonPointersTest, pointerWithFormat)
{
    const auto format = Qt::ISODate;
    auto date = QDateTime::fromString("2019-11-21T12:22:48", format);
    const QDateTime* pointer{&date};

    QJsonObject expected{{"date", json::toValueWithFormat(date, format)}};
    QJsonObject json{{"date", json::toValueWithFormat(pointer, format)}};

    const auto expectedStr = json::toByteArray(expected).toStdString();
    const auto resultStr = json::toByteArray(json).toStdString();

    EXPECT_EQ(expectedStr, resultStr);
}

TEST(JsonPointersTest, vectorWithPointer)
{
    TestStruct fisrtObject{.name = "firstName", .value = "firstValue"};
    TestStruct secondObject{.name = "secondName", .value = "secondValue"};

    std::vector<const TestStruct*> objects{{&fisrtObject, &secondObject}};

    QJsonObject expected{
        {"objects",

         QJsonArray{
             QJsonObject{
                 {"name", "firstName"},
                 {"value", "firstValue"},
             },
             QJsonObject{
                 {"name", "secondName"},
                 {"value", "secondValue"},
             },
         }},
    };
    QJsonObject json{{"objects", json::toValue(objects)}};

    const auto expectedStr = json::toByteArray(expected).toStdString();
    const auto resultStr = json::toByteArray(json).toStdString();

    EXPECT_EQ(expectedStr, resultStr);
}

TEST(JsonPointersTest, vectorWithNullptr)
{
    TestStruct fisrtObject{.name = "firstName", .value = "firstValue"};
    TestStruct secondObject{.name = "secondName", .value = "secondValue"};

    std::vector<const TestStruct*> objects{
        &fisrtObject,
        &secondObject,
        nullptr,
    };

    QJsonObject expected{
        {"objects",

         QJsonArray{
             QJsonObject{
                 {"name", "firstName"},
                 {"value", "firstValue"},
             },
             QJsonObject{
                 {"name", "secondName"},
                 {"value", "secondValue"},
             },
             QJsonValue{},
         }},
    };
    QJsonObject json{{"objects", json::toValue(objects)}};

    const auto expectedStr = json::toByteArray(expected).toStdString();
    const auto resultStr = json::toByteArray(json).toStdString();

    EXPECT_EQ(expectedStr, resultStr);
}

} // namespace json::tests
