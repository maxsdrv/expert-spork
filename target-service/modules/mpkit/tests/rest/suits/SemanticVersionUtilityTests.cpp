#include "fixtures/SemanticVersionUtilityFixture.h"

/* TEST PLAN:

1. Semantic version form string
2. Semantic version to string
3. Semantic version to number
4. Item version to number
5. Item version with text to number
6. Semantic version form string with string

*/

namespace mpk::rest::tests
{

class SemanticVersionUtilityTests : public SemanticVersionUtilityFixture
{
};

TEST_F(SemanticVersionUtilityTests, semanticVersionFromString)
{
    const SemanticVersion version = fromString("1.2.3");

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
}

TEST_F(SemanticVersionUtilityTests, semanticVersionToString)
{
    const SemanticVersion version{.major = 1, .minor = 2, .patch = 3};

    EXPECT_EQ("1.2.3", toString(version));
}

TEST_F(SemanticVersionUtilityTests, semanticVersionToNumber)
{
    const SemanticVersion version{.major = 1, .minor = 2, .patch = 3};

    EXPECT_EQ(1'002'003, toNumber(version));
}

TEST_F(SemanticVersionUtilityTests, itemVersionToNumber)
{
    EXPECT_EQ(1, toNumber("1"));
}

TEST_F(SemanticVersionUtilityTests, itemVersionWithTextToNumber)
{
    EXPECT_EQ(1, toNumber("1-beta"));
}

TEST_F(SemanticVersionUtilityTests, semanticVersionFromStringWithString)
{
    const SemanticVersion version = fromString("v1.2.3-beta");

    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
}

} // namespace mpk::rest::tests
