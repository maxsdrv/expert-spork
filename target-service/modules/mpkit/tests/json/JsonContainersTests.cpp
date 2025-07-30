#include "gqtest"

#include "TestSerialization.h"

#include "json/composers/Containers.h"
#include "json/composers/General.h"
#include "json/composers/QByteArray.h"
#include "json/composers/QMap.h"
#include "json/parsers/Containers.h"
#include "json/parsers/General.h"
#include "json/parsers/QByteArray.h"
#include "json/parsers/QMap.h"

#include <QCryptographicHash>
#include <QSet>

/*
 * testVector
 * testList
 * testMap
 * testUnorderedMap
 * testMultiMap
 * testUnorderedMultiMap
 * testStringMultiMap
 * testSet
 * testQList
 * testQVector
 * testQStringList
 * testQSet
 * testQMap
 * testStdArray
 */

namespace json::tests
{

TEST(JsonTest, testVector)
{
    std::vector<int> expected = {1, 2, 5, 0};
    testSerialization(expected);
}

TEST(JsonTest, testList)
{
    std::list<int> expected = {1, 2, 5, 0};
    testSerialization(expected);
}

TEST(JsonTest, testMap)
{
    std::map<std::string, int> expected = {{"1", 1}, {"5", 5}};
    testSerialization(expected);
}

TEST(JsonTest, testUnorderedMap)
{
    std::unordered_map<std::string, int> expected = {{"1", 1}, {"5", 5}};
    testSerialization(expected);
}

TEST(JsonTest, testMultiMap)
{
    std::multimap<std::string, int> expected = {{"1", 1}, {"5", 5}};
    testSerialization(expected);
}

TEST(JsonTest, testUnorderedMultiMap)
{
    std::unordered_multimap<std::string, int> expected = {{"1", 1}, {"5", 5}};
    testSerialization(expected);
}

TEST(JsonTest, testStringMultiMap)
{
    using Map = std::multimap<std::string, std::string>;

    Map mapExpected = {{"int", "1"}, {"double", "1.1"}, {"bool", "false"}, {"string", "str"}};
    QJsonValue jsonExpected =
        QJsonObject{{"int", 1}, {"double", 1.1}, {"bool", false}, {"string", "str"}};
    auto mapActual = json::fromValue<Map>(jsonExpected);
    EXPECT_EQ(mapActual, mapExpected);
    auto jsonActual = json::toValue(mapExpected);
    EXPECT_EQ(jsonActual, jsonExpected);
}

TEST(JsonTest, testSet)
{
    std::set<std::string> expected = {"one", "two"};
    testSerialization(expected);
}

TEST(JsonTest, testQList)
{
    QList<int> expectedInt = {1, 3, 6, 21};
    testSerialization(expectedInt);
    QList<QByteArray> expectedByteArray;
    for (int i = 0; i < 20; i++)
    {
        expectedByteArray.append(QCryptographicHash::hash(
            QString("Hello, world(%1)!").arg(i).toLocal8Bit(), QCryptographicHash::Md5));
    }
    testSerialization(expectedByteArray);
}

TEST(JsonTest, testQVector)
{
    QVector<int> expected = {1, 3, 6, 21};
    testSerialization(expected);
}

TEST(JsonTest, testQStringList)
{
    testSerialization(QStringList{"one", "two", "three"});
}

TEST(JsonTest, testQMap)
{
    QMap<int, QString> expected = {{1, "odin"}, {2, "dva"}};
    testSerialization(expected);
}

TEST(JsonTest, testQSet)
{
    testSerialization(QSet<QString>{"1", "3", "6", "21"});
}

TEST(JsonTest, testStdArray)
{
    testSerialization(std::array{1, 3, 6, 21});
}

} // namespace json::tests
