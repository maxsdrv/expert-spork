#pragma once

#include "dss-core/DeviceIdMappingStorage.h"
#include "dss-core/DevicePositionStorage.h"
#include "dss-core/JammerGroupProvider.h"
#include "dss-core/license/LicenseController.h"

#include <gsl/pointers>

#include <memory>

namespace mpk::dss::backend
{

struct BackendComponents
{
    std::shared_ptr<core::DeviceIdMappingStorage> deviceIdMappingStorage;
    std::shared_ptr<core::DevicePositionStorage> devicePositionStorage;
    gsl::not_null<core::JammerGroupProvider*> jammerGroupProvider;
    gsl::not_null<core::LicenseController*> licenseController;
};

} // namespace mpk::dss::backend
