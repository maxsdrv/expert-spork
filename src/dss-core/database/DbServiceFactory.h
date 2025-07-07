#pragma once

#include "dss-core/database/DbService.h"

#include <memory>

namespace mpk::dss::core
{

std::shared_ptr<DbService> createDbService();

} // namespace mpk::dss::core
