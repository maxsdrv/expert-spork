/** @file
 * @brief Defines severity level here
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "mpklog_export.h"

#include <iostream>

namespace appkit::logger
{

enum class SeverityLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

// The operator puts a human-friendly representation of the severity level to the stream
std::string toString(SeverityLevel level) MPKLOG_EXPORT;
std::ostream& operator<<(std::ostream& stream, SeverityLevel level) MPKLOG_EXPORT;

// The operator converts std::string to SeverityLevel
SeverityLevel toLevel(std::string value) MPKLOG_EXPORT;
std::istream& operator>>(std::istream& stream, SeverityLevel& level) MPKLOG_EXPORT;

} // namespace appkit::logger
