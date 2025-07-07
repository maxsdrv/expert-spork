#pragma once

#include "exception/InvalidConfigEntry.h"

namespace mpk::dss::backend::exception
{

class InvalidConfigValue : public ::exception::InvalidConfigEntry
{
};

} // namespace mpk::dss::backend::exception
