#pragma once

#include <boost/serialization/strong_typedef.hpp>

#include <cstdint>
#include <string>

namespace rpc
{

using MethodName = std::string;
using RequestId = int32_t;

enum class ReturnCode
{
    UNDEFINED = -1,
    SUCCESS = 0,
    INVALID_METHOD,
    INVALID_ARGS,
    INTERNAL_ERROR,
    TIMEOUT
};

} // namespace rpc
