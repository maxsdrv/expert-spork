#pragma once

#include <string>

namespace mpk::drone
{

enum class CommandType
{
    READ = 0,
    SET,
    UPDATE_SOFTWARE,
    UPDATE_LICENSE
};

CommandType commandTypeFromString(const std::string& str);
std::string commandTypeToString(CommandType value);

} // namespace mpk::drone
