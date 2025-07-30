/**
 * @file
 * @brief Invalid plugin exception
 *
 *
 * $Id: $
 *
 **/

#pragma once

#include "General.h"

namespace exception
{

/**
 * This exception is thrown when loading plugin failed unexpectedly
 **/
class InvalidPlugin : public virtual General
{
};

} // namespace exception
