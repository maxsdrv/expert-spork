/** @file
 * @brief
 *
 * @ingroup
 *
 * $Id: $
 */

#pragma once

#include "General.h"

namespace exception
{

/**
 * Type for passing path to exception.
 */
using PathInfo = boost::error_info<struct pathInfo_, std::string>;

/**
 * This exception is thrown when malformed path encountered
 **/
class InvalidPath : public virtual General
{
public:
    explicit InvalidPath(const std::string& path)
    {
        (*this) << PathInfo(path);
    }
};

} // namespace exception
