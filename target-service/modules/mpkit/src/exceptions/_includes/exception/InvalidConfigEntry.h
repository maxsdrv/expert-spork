/**
 * @file
 * @brief Configuration invalid entry exception
 *
 *
 * $Id: $
 *
 **/

#pragma once

#include "Config.h"

namespace exception
{

/**
 * This exception is thrown when parser found invalid entry
 * in configuration file
 **/
class InvalidConfigEntry : public virtual Config
{
};

} // namespace exception
