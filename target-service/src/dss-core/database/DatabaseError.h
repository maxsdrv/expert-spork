#pragma once

#include "exception/General.h"

#include <QString>

namespace exception
{

using DatabaseErrorText = boost::error_info<struct databaseErrorText_, QString>;

class DatabaseError : public virtual General
{
};

} // namespace exception
