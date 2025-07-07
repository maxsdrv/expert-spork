#pragma once

#include "exception/MissingEntity.h"

namespace mpk::dss::backend::exception
{

class MissingParser : public ::exception::MissingEntity
{
};

} // namespace mpk::dss::backend::exception
