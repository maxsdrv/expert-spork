#include "mpk/rest/SemanticVersionUtility.h"
#include "mpk/rest/SemanticVersion.h"

#include "log/Log2.h"

#include <vector>

namespace mpk::rest
{

std::string SemanticVersionUtility::toString(const SemanticVersion& version)
{
    const auto major = std::to_string(version.major);
    const auto minor = std::to_string(version.minor);
    const auto patch = std::to_string(version.patch);

    const std::string result = major + "." + minor + "." + patch;
    return result;
}

SemanticVersion SemanticVersionUtility::fromString(const std::string& value)
{
    const char seperator = '.';

    size_t pos = 0;
    std::string string = value;
    std::vector<std::string> tokens;

    try
    {
        while ((pos = string.find(seperator)) != std::string::npos)
        {
            std::string token = string.substr(0, pos);
            tokens.push_back(token);
            string.erase(0, pos + 1);
        }

        tokens.push_back(string);

        unsigned major = toNumber(tokens.at(0));
        unsigned minor = toNumber(tokens.at(1));
        unsigned patch = toNumber(tokens.at(2));

        return SemanticVersion{major, minor, patch};
    }
    catch (const std::exception& exception)
    {
        LOG_ERROR << "Parse version " << value << ", " << exception.what();
        return SemanticVersion{0, 0, 0};
    }
}

unsigned SemanticVersionUtility::toNumber(const SemanticVersion& version)
{
    const unsigned major = version.major * 1'000 * 1'000;
    const unsigned minor = version.minor * 1'000;
    const unsigned patch = version.patch;

    return major + minor + patch;
}

unsigned SemanticVersionUtility::toNumber(const std::string& value)
{
    std::string numberString;
    for (char charter: value)
    {
        if (std::isdigit(charter) != 0)
        {
            numberString += charter;
        }
    }

    try
    {
        return std::stoi(numberString);
    }
    catch (const std::exception& exception)
    {
        LOG_ERROR << "Parse version " << numberString << ", " << exception.what();
        return 0;
    }
}

} // namespace mpk::rest
