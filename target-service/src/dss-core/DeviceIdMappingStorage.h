#pragma once

#include "dss-core/database/DBStorage.h"
#include "dss-core/database/DeviceIdMappingQueryHelper.h"

namespace mpk::dss::core
{

class DbService;

using DeviceIdMappingStorage = DBStorage<DeviceIdMapping>;
using DeviceIdMappingStorageTraits = DBStorageTraits<DeviceIdMapping>;
using DeviceIdMappingProvider =
    Provider<DeviceIdMappingStorage, DeviceIdMappingStorageTraits>;

} // namespace mpk::dss::core
