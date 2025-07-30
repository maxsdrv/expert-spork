/**
 * @file
 * @brief Input/output file exception
 *
 * $Id: $
 *
 **/

#pragma once

#include "StreamError.h"

#include <filesystem>

namespace exception
{

/**
 * This exception is thrown in case of failed file input/output operations
 **/
class FileError : public virtual StreamError
{
};

/**
 * Type for passing file info to exception.
 */
using FileInfo = boost::error_info<struct valueInfo_, std::filesystem::path>;

} // namespace exception
