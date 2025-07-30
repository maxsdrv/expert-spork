/**
 * @file
 * @brief Duplicate entity exception
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
 * This exception is thrown when double insertion of unique type is performed
 **/
class DuplicateEntity : public virtual General
{
};

} // namespace exception
