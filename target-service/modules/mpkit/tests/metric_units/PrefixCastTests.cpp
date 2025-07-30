#include "metric_units/MetricUnits.h"

#include <gmock/gmock.h>

using namespace mpk::metric_units;
using namespace mpk::metric_units::prefix;

TEST(PrefixCastTests, convertDown)
{
    auto r1 = prefix_cast<Milli, Micro>(1234.5F);
    EXPECT_FLOAT_EQ(1234500.F, r1);

    auto r2 = prefix_cast<Mega, Deca>(1234567ULL);
    EXPECT_EQ(123456700000ULL, r2);

    auto r3 = prefix_cast<Kilo, Centi>(1.23456789);
    EXPECT_DOUBLE_EQ(123456.789, r3);
}

TEST(PrefixCastTests, convertUp)
{
    auto r1 = prefix_cast<Micro, Milli>(1234.5F);
    EXPECT_FLOAT_EQ(1.234500F, r1);

    auto r2 = prefix_cast<Deca, Mega>(1234567);
    EXPECT_EQ(12, r2);

    auto r3 = prefix_cast<Centi, Kilo>(1.23456789);
    EXPECT_DOUBLE_EQ(0.0000123456789, r3);
}

TEST(PrefixCastTests, convertEqual)
{
    auto r1 = prefix_cast<Micro, Micro>(1234.5F);
    EXPECT_FLOAT_EQ(1234.5F, r1);

    auto r2 = prefix_cast<Mega, Mega>(1234567);
    EXPECT_EQ(1234567, r2);

    auto r3 = prefix_cast<Centi, Centi>(1.23456789);
    EXPECT_DOUBLE_EQ(1.23456789, r3);
}
