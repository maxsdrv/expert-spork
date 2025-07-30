/** @file
 * @brief     Comparators test suite
 *
 *
 * $Id: $
 */

#include "gqtest"

#include "Comparators.h"

#include <QVariant>

namespace gqtest
{

TEST(ComparatorsTest, equalStrings)
{
    EXPECT_TRUE(isEqual(QString("abc"), QString("abc")));
}

TEST(ComparatorsTest, notEqualStrings)
{
    EXPECT_FALSE(isEqual(QString("abc"), QString("cba")));
}

TEST(ComparatorsTest, equalMap)
{
    QMap<int, QString> mapOne;
    mapOne.insert(1, "One");
    mapOne.insert(2, "Two");

    QMap<int, QString> mapTwo;
    mapTwo.insert(2, "Two");
    mapTwo.insert(1, "One");

    EXPECT_TRUE(isEqual(mapOne, mapTwo));
    EXPECT_TRUE(isEqual(mapTwo, mapOne));
}

TEST(ComparatorsTest, mapSizeMismatch)
{
    QMap<int, QString> mapOne;
    mapOne.insert(1, "One");

    QMap<int, QString> mapTwo;
    mapTwo.insert(2, "Two");
    mapTwo.insert(1, "One");

    EXPECT_FALSE(isEqual(mapOne, mapTwo));
    EXPECT_FALSE(isEqual(mapTwo, mapOne));
}

TEST(ComparatorsTest, mapKeyMismatch)
{
    QMap<int, QString> mapOne;
    mapOne.insert(1, "One");
    mapOne.insert(22, "Two");

    QMap<int, QString> mapTwo;
    mapTwo.insert(2, "Two");
    mapTwo.insert(1, "One");

    EXPECT_FALSE(isEqual(mapOne, mapTwo));
    EXPECT_FALSE(isEqual(mapTwo, mapOne));
}

TEST(ComparatorsTest, mapValueMismatch)
{
    QMap<int, QString> mapOne;
    mapOne.insert(1, "One");
    mapOne.insert(2, "TwoTwo");

    QMap<int, QString> mapTwo;
    mapTwo.insert(2, "Two");
    mapTwo.insert(1, "One");

    EXPECT_FALSE(isEqual(mapOne, mapTwo));
    EXPECT_FALSE(isEqual(mapTwo, mapOne));
}

TEST(ComparatorsTest, equalList)
{
    QList<int> listOne;
    listOne << 1 << 2 << 3;
    QList<int> listTwo;
    listTwo << 1 << 2 << 3;

    EXPECT_TRUE(isEqual(listOne, listTwo));
}

TEST(ComparatorsTest, listSizeMismatch)
{
    QList<int> listOne;
    listOne << 1 << 2 << 3;
    QList<int> listTwo;
    listTwo << 1 << 2;

    EXPECT_FALSE(isEqual(listOne, listTwo));
    EXPECT_FALSE(isEqual(listTwo, listOne));
}

TEST(ComparatorsTest, listValueMismatch)
{
    QList<int> listOne;
    listOne << 1 << 2 << 3;
    QList<int> listTwo;
    listTwo << 1 << 3 << 2;

    EXPECT_FALSE(isEqual(listOne, listTwo));
    EXPECT_FALSE(isEqual(listTwo, listOne));
}

TEST(ComparatorsTest, fuzzyStringEqual)
{
    QString stringOne("1,2,3,4,5");
    QString stringTwo("1,2,3,4,5");

    EXPECT_TRUE(fuzzyStringCompare(stringOne, stringTwo));
}

TEST(ComparatorsTest, fuzzyStringPermutated)
{
    QString stringOne("5,3,2,4,1");
    QString stringTwo("1,2,3,4,5");

    EXPECT_TRUE(fuzzyStringCompare(stringOne, stringTwo));
    EXPECT_TRUE(fuzzyStringCompare(stringTwo, stringOne));
}

TEST(ComparatorsTest, fuzzyStringPermutatedIgnoreWhitespace)
{
    QString stringOne("5#   3   # 2# 4# 1");
    QString stringTwo("1#2#3#4#5");

    EXPECT_TRUE(fuzzyStringCompare(stringOne, stringTwo, "#", true));
    EXPECT_TRUE(fuzzyStringCompare(stringTwo, stringOne, "#", true));

    EXPECT_FALSE(fuzzyStringCompare(stringOne, stringTwo, "#", false));
    EXPECT_FALSE(fuzzyStringCompare(stringTwo, stringOne, "#", false));
}

TEST(ComparatorsTest, fuzzyStringMismatch)
{
    QString stringOne("1,2,3,4,5");
    QString stringTwo("1,2,6,4,5");

    EXPECT_FALSE(fuzzyStringCompare(stringOne, stringTwo));
}

TEST(ComparatorsTest, objectsEqual)
{
    QObject objectOne;
    objectOne.setObjectName("test");
    objectOne.setProperty("property", 100);

    QObject objectTwo;
    objectTwo.setObjectName("test");
    objectTwo.setProperty("property", 100);
    EXPECT_TRUE(isEqual(objectOne, objectTwo));
}

TEST(ComparatorsTest, objectsOwnPropertyMismatch)
{
    QObject objectOne;
    objectOne.setObjectName("test");
    objectOne.setProperty("property", 100);

    QObject objectTwo;
    objectTwo.setObjectName("not test");
    objectTwo.setProperty("property", 100);
    EXPECT_FALSE(isEqual(objectOne, objectTwo));
    EXPECT_FALSE(isEqual(objectTwo, objectOne));
}

TEST(ComparatorsTest, objectsDynamicPropertyMismatch)
{
    QObject objectOne;
    objectOne.setObjectName("test");
    objectOne.setProperty("property", 101);

    QObject objectTwo;
    objectTwo.setObjectName("test");
    objectTwo.setProperty("property", 100);
    EXPECT_FALSE(isEqual(objectOne, objectTwo));
    EXPECT_FALSE(isEqual(objectTwo, objectOne));
}

} // namespace gqtest
