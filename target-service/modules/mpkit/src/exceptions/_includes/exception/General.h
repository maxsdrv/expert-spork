/**
 * @file
 * @brief Basic exception
 *
 *
 * $Id: $
 *
 **/

#pragma once

#include "ErrorInfo.h"

#include <boost/exception/all.hpp>
#include <exception>

namespace exception
{

/**
 * Basic class for exceptions hierarchy
 **/
class General : public virtual boost::exception, public virtual std::exception
{
public:
    const char* what() const noexcept override
    {
        return boost::diagnostic_information_what(*this);
    }
};

/**
 * Get info from exception, return default value if exception does not provide
 * info
 */
template <typename T, typename R, typename E>
R info(const E& ex, R defaultValue)
{
    if (const R* value = boost::get_error_info<T>(ex))
    {
        return *value;
    }
    return defaultValue;
}

} // namespace exception
