/**
 * @file
 * @brief Base stream exception
 *
 * $Id: $
 *
 **/

#pragma once

#include "General.h"

namespace exception
{

/**
 * Base class for i/o stream exceptions
 **/
class StreamError : public virtual General
{
};

} // namespace exception
