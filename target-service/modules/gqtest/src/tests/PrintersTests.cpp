/** @file
 * @brief     Comparators test suite
 *
 *
 * $Id: $
 */

#include "gqtest"

namespace gqtest
{

TEST(PrintersTest, printQString)
{
    std::stringstream str;
    PrintTo(QString("1AB"), &str);

    EXPECT_EQ(R"(1AB)", str.str());
}

TEST(PrintersTest, printQByteArray)
{
    std::stringstream str;

    PrintTo(QByteArray("1AB"), &str);

    EXPECT_EQ("QByteArray(314142)", str.str());
}

TEST(PrintersTest, printQPair)
{
    std::stringstream str;
    PrintTo(QPair<int, QString>{ 1, "one" }, &str);

    EXPECT_EQ(R"(QPair(1, one))", str.str());
}

TEST(PrintersTest, printQList)
{
    std::stringstream str;

    PrintTo(
        QList<QString>() << "one"
                         << "two"
                         << "three",
        &str);

    EXPECT_EQ(R"(QList(one, two, three))", str.str());
}

TEST(PrintersTest, printQMap)
{
    std::stringstream str;

    const QMap<int, QByteArray> map = { { 1, "ONE" }, { 2, "TWO" } };

    PrintTo(map, &str);

    EXPECT_EQ(
        R"(QMap((1, QByteArray(4F4E45)), (2, QByteArray(54574F))))", str.str());
}

} // namespace gqtest
