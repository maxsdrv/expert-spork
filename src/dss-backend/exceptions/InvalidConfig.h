#pragma once

#include "exception/InvalidConfigEntry.h"

namespace mpk::dss::backend::exception
{

class InvalidConfig : public ::exception::InvalidConfigEntry
{
};

} // namespace mpk::dss::backend::exception
