#include "SemanticVersionUtilityFixture.h"

#include "mpk/rest/SemanticVersionUtility.h"

namespace mpk::rest::tests
{

std::string SemanticVersionUtilityFixture::toString(
    const SemanticVersion& version)
{
    return SemanticVersionUtility::toString(version);
}

SemanticVersion SemanticVersionUtilityFixture::fromString(
    const std::string& value)
{
    return SemanticVersionUtility::fromString(value);
}

unsigned SemanticVersionUtilityFixture::toNumber(const SemanticVersion& version)
{
    return SemanticVersionUtility::toNumber(version);
}

unsigned SemanticVersionUtilityFixture::toNumber(const std::string& value)
{
    return SemanticVersionUtility::toNumber(value);
}

} // namespace mpk::rest::tests
