#include "metric_units/MetricUnits.h"

#include <gmock/gmock.h>

using namespace mpk::metric_units;
using namespace mpk::metric_units::helpers;
using namespace mpk::metric_units::prefix;

TEST(PrefixConverterTests, convertName)
{
    auto r1 = Converter<Deci, ListAll, float, Name>::fromName(1234.5F, "u");
    EXPECT_FLOAT_EQ(0.012345F, r1);

    auto r2 = Converter<Micro, ListAll, unsigned long long, Name>::fromName(
        3ULL, "da");
    EXPECT_EQ(30000000ULL, r2);

    auto r3 = Converter<Giga, ListAll, double, Name>::fromName(48., "M");
    EXPECT_DOUBLE_EQ(0.048, r3);

    auto r4 = Converter<Giga, ListStandard, double, Name>::fromName(48., "k");
    EXPECT_DOUBLE_EQ(0.000048, r4);
}
