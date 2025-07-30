#pragma once

#include "dss-core/database/DBStorage.h"
#include "dss-core/database/DevicePositionQueryHelper.h"

namespace mpk::dss::core
{

class DbService;

using DevicePositionStorage = DBStorage<DevicePositionExtended>;
using DevicePositionStorageTraits = DBStorageTraits<DevicePositionExtended>;
using DevicePositionProvider =
    Provider<DevicePositionStorage, DevicePositionStorageTraits>;

} // namespace mpk::dss::core
