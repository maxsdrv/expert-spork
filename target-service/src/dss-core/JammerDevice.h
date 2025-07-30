#pragma once

#include "dss-core/Device.h"
#include "dss-core/DeviceProvider.h"
#include "dss-core/DeviceStorage.h"
#include "dss-core/JammerProperties.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::dss::core
{

using JammerDevice = Device<JammerProperties>;
using JammerProvider = DeviceProvider<JammerProperties>;
using JammerStorage = DeviceStorage<JammerProperties>;
using JammerProviderPtr = gsl::not_null<JammerProvider*>;
using JammerProviderPtrs = std::vector<JammerProviderPtr>;

} // namespace mpk::dss::core
