/**
 * @file
 * @brief Missing entity exception
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
 * This exception is thrown when search by key failed
 **/
class MissingEntity : public virtual General
{
};

} // namespace exception
