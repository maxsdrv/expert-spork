#pragma once

#include <string>

namespace mpk::rest
{

struct SemanticVersion;

class SemanticVersionUtility
{
public:
    static std::string toString(const SemanticVersion& version);
    static SemanticVersion fromString(const std::string& value);

    static unsigned toNumber(const SemanticVersion& version);
    static unsigned toNumber(const std::string& value);
};

} // namespace mpk::rest
