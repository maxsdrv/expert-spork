#pragma once

#include "dss-core/CameraProperties.h"
#include "dss-core/CameraProvider.h"
#include "dss-core/Device.h"
#include "dss-core/DeviceProvider.h"
#include "dss-core/DeviceStorage.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::dss::core
{

using CameraDevice = Device<CameraProperties>;
using CameraProviderPtr = gsl::not_null<CameraProvider*>;
using CameraProviderPtrs = std::vector<CameraProviderPtr>;

} // namespace mpk::dss::core
