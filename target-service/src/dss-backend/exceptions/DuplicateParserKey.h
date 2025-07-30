#pragma once

#include "exception/DuplicateEntity.h"

namespace mpk::dss::backend::exception
{

class DuplicateParserKey : public ::exception::DuplicateEntity
{
};

} // namespace mpk::dss::backend::exception
