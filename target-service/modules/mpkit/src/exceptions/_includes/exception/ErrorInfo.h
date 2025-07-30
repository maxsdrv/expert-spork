/** @file
 * @brief Commonly used error_info
 *
 * @ingroup
 *
 * $Id: $
 */

#pragma once

#include <boost/exception/all.hpp>
#include <boost/format.hpp>

namespace exception
{

/**
 * Type for passing extra details to exception. One can use it to pass
 * arbitrary string to exception
 */
using ExceptionInfo = boost::error_info<struct exceptionInfo_, std::string>;

/**
 * Combine type of exception and it's description
 */
struct ForeignExceptionDetails
{
    std::string type; // Type of caught exception
    std::string what; // String returned by "what" method of caught exception
};

/**
 * This type can be used for passing info from caught exception to new one
 */
using ForeignExceptionInfo = boost::error_info<struct caughtException_, ForeignExceptionDetails>;

/**
 * Convert exception to ForeignExceptionInfo
 */
inline ForeignExceptionInfo toForeignExceptionInfo(const std::exception& ex)
{
    ForeignExceptionDetails details;
    details.type = typeid(ex).name();
    details.what = ex.what();
    return {details};
}

/**
 * Stream operator for ForeignExceptionDetails
 */
inline std::ostream& operator<<(std::ostream& stream, const ForeignExceptionDetails& details)
{
    stream << (boost::format("type: [%1$s], what: [%2$s]") % details.type % details.what).str();
    return stream;
}

} // namespace exception
