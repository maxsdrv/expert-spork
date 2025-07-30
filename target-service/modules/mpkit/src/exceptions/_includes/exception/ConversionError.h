#pragma once

#include "exception/StringParseFailed.h"

namespace exception
{

// TODO: Kept for backward compatibility
using ConversionError = StringParseFailed<std::string>;

} // namespace exception
