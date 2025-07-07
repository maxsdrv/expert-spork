#pragma once

#include "dss-core/Device.h"
#include "dss-core/DeviceProvider.h"
#include "dss-core/DeviceStorage.h"
#include "dss-core/SensorProperties.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::dss::core
{

using SensorDevice = Device<SensorProperties>;
using SensorProvider = DeviceProvider<SensorProperties>;
using SensorStorage = DeviceStorage<SensorProperties>;
using SensorProviderPtr = gsl::not_null<SensorProvider*>;
using SensorProviderPtrs = std::vector<SensorProviderPtr>;

} // namespace mpk::dss::core
