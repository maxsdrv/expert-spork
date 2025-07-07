#pragma once

#include "dss-backend/rest/geolocation/Geolocation.h"
#include "dss-core/DevicePosition.h"

namespace mpk::dss::backend::rest
{

struct DevicePositionVsGeolocation
{
    static core::DevicePosition convert(const Geolocation& geolocation);
    static Geolocation get(const core::DevicePosition& devicePosition);
};

} // namespace mpk::dss::backend::rest
