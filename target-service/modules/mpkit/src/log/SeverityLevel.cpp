/** @file
 * @brief Defines severity level here
 *
 * @ingroup
 *
 * $Id: $
 */

#include "log/SeverityLevel.h"

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <typeinfo>
#include <utility>
#include <vector>

namespace appkit
{

namespace logger
{

namespace
{

using Level = std::pair<SeverityLevel, std::string>;
using Levels = std::vector<Level>;

Levels getLevels()
{
    static Levels levels;
    if (levels.empty())
    {
        levels.push_back(std::make_pair(SeverityLevel::Debug, "DEBUG"));
        levels.push_back(std::make_pair(SeverityLevel::Info, "INFO"));
        levels.push_back(std::make_pair(SeverityLevel::Warning, "WARNING"));
        levels.push_back(std::make_pair(SeverityLevel::Error, "ERROR"));
        levels.push_back(std::make_pair(SeverityLevel::Fatal, "FATAL"));
        levels.push_back(std::make_pair(SeverityLevel::Trace, "TRACE"));
    }
    return levels;
}

} // namespace

std::ostream& operator<<(std::ostream& stream, SeverityLevel level)
{
    stream << toString(level);
    return stream;
}

std::istream& operator>>(std::istream& stream, SeverityLevel& level)
{
    std::string value;
    stream >> value;
    level = toLevel(value);
    return stream;
}

std::string toString(SeverityLevel level)
{
    const Levels& levels = getLevels();
    // NOLINTNEXTLINE
    auto it = boost::find_if(levels, boost::bind(&Level::first, _1) == level);
    if (it != levels.end())
    {
        return it->second;
    }
    throw std::runtime_error("No conversion from SeverityLevel");
}

SeverityLevel toLevel(std::string value)
{
    boost::trim(value);
    const Levels& levels = getLevels();
    // NOLINTNEXTLINE
    auto it = boost::find_if(levels, boost::bind(&Level::second, _1) == value);
    if (it != levels.end())
    {
        return it->first;
    }
    throw std::runtime_error("No conversion to SeverityLevel");
}

} // namespace logger

} // namespace appkit
