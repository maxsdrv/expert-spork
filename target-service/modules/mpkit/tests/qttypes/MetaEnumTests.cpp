#include "qttypes/MetaEnum.h"

#include <gtest/gtest.h>

namespace tests
{

template <typename T>
class MetaEnumTest : public testing::Test
{
};

struct TestEnum
{
    Q_GADGET

public:
    enum class Value
    {
        One,
        Two
    };
    Q_ENUM(Value)
};

struct TestEnumClass
{
    Q_GADGET

public:
    enum Value
    {
        One,
        Two
    };
    Q_ENUM(Value)
};

using EnumTests = testing::Types<TestEnum, TestEnumClass>;
TYPED_TEST_SUITE(MetaEnumTest, EnumTests);

TYPED_TEST(MetaEnumTest, toString)
{
    EXPECT_EQ("One", qt::metaEnumToString(TestEnum::Value::One));
}

TYPED_TEST(MetaEnumTest, fromValidString)
{
    EXPECT_EQ(TestEnum::Value::Two, qt::metaEnumFromString<TestEnum::Value>("Two"));
}

TYPED_TEST(MetaEnumTest, fromInvalidString)
{
    EXPECT_THROW(
        qt::metaEnumFromString<TestEnum::Value>("three"), exception::StringParseFailed<QString>);
    EXPECT_THROW(
        qt::metaEnumFromString<TestEnum::Value>("two"), exception::StringParseFailed<QString>);
}

} // namespace tests

#include "MetaEnumTests.moc"
