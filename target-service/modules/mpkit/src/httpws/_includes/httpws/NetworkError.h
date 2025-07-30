#pragma once

#include "exception/General.h"

namespace exception
{

/**
 * This exception is throw in case of network error
 **/
class NetworkError : public virtual General
{
};

/**
 * Type for passing network info to exception.
 */
using NetworkInfo = boost::error_info<struct valueInfo_, std::string>;

} // namespace exception
