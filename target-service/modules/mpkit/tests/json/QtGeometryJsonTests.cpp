#include "gqtest"

#if __cplusplus >= 202002L
#include "json/composers/QtGeometry.h"
#include "json/parsers/QtGeometry.h"

template <mpk::concepts::QtLine L>
std::ostream& operator<<(std::ostream& stream, const L& line)
{
    stream << QStringLiteral("QtLine(%1, %2, %3, %4)")
                  .arg(line.x1())
                  .arg(line.y1())
                  .arg(line.x2())
                  .arg(line.y2())
                  .toStdString();
    return stream;
}

template <mpk::concepts::QtPoint P>
std::ostream& operator<<(std::ostream& stream, const P& point)
{
    stream << QStringLiteral("QtPoint(%1, %2)").arg(point.x()).arg(point.y()).toStdString();
    return stream;
}

template <mpk::concepts::QtSize S>
std::ostream& operator<<(std::ostream& stream, const S& size)
{
    stream << QStringLiteral("QtSize(%1, %2)").arg(size.width()).arg(size.height()).toStdString();
    return stream;
}

template <mpk::concepts::QtRect R>
std::ostream& operator<<(std::ostream& stream, const R& rect)
{
    stream << QStringLiteral("QtRect(%1, %2, %3, %4)")
                  .arg(rect.x())
                  .arg(rect.y())
                  .arg(rect.width())
                  .arg(rect.height())
                  .toStdString();
    return stream;
}

namespace json::tests
{

template <typename T>
void testPrimitiveParsing(const QJsonObject& object, const T& expected)
{
    auto actual = json::fromValue<T>(object);
    EXPECT_EQ(expected, actual);

    auto afterComposeAndParse = json::fromValue<T>(json::toValue(expected));
    EXPECT_EQ(expected, afterComposeAndParse);
}

TEST(QtGeometryParserTest, qLine)
{
    testPrimitiveParsing({{"x1", 10}, {"y1", 20}, {"x2", 20}, {"y2", 35}}, QLine{10, 20, 20, 35});
}

TEST(QtGeometryParserTest, qLineF)
{
    testPrimitiveParsing(
        {{"x1", 10.1}, {"y1", 20.2}, {"x2", 20.5}, {"y2", 35.8}}, QLineF{10.1, 20.2, 20.5, 35.8});
}

TEST(QtGeometryParserTest, qPoint)
{
    testPrimitiveParsing({{"x", 10}, {"y", 20}}, QPoint{10, 20});
}

TEST(QtGeometryParserTest, qPointF)
{
    testPrimitiveParsing({{"x", 10.1}, {"y", 20.2}}, QPointF{10.1, 20.2});
}

TEST(QtGeometryParserTest, qRect)
{
    testPrimitiveParsing(
        {{"x", 10}, {"y", 20}, {"width", 30}, {"height", 40}}, QRect{10, 20, 30, 40});
}

TEST(QtGeometryParserTest, qRectF)
{
    testPrimitiveParsing(
        {{"x", 10.1}, {"y", 20.2}, {"width", 30.5}, {"height", 40.1}},
        QRectF{10.1, 20.2, 30.5, 40.1});
}

TEST(QtGeometryParserTest, invalidRRect)
{
    EXPECT_THROW(
        Q_UNUSED(json::fromValue<QRect>(
            QJsonObject{{"x", 10}, {"y", 20}, {"width", -30}, {"height", 40}})),
        exception::json::InvalidValue);
}

TEST(QtGeometryParserTest, qSize)
{
    testPrimitiveParsing({{"width", 10}, {"height", 20}}, QSize{10, 20});
}

TEST(QtGeometryParserTest, qSizeF)
{
    testPrimitiveParsing({{"width", 10.1}, {"height", 20.2}}, QSizeF{10.1, 20.2});
}

} // namespace json::tests
#endif
