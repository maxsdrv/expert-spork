#pragma once

#include "mpk/rest/SemanticVersion.h"

#include <gtest/gtest.h>

namespace mpk::rest::tests
{

class SemanticVersionUtilityFixture : public ::testing::Test
{
public:
    std::string toString(const SemanticVersion& version);
    SemanticVersion fromString(const std::string& value);

    unsigned toNumber(const SemanticVersion& version);
    unsigned toNumber(const std::string& value);
};

} // namespace mpk::rest::tests
