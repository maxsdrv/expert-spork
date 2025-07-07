#include "DevicePositionVsGeolocation.h"

namespace mpk::dss::backend::rest
{

core::DevicePosition DevicePositionVsGeolocation::convert(
    const Geolocation& geolocation)
{
    return core::DevicePosition{geolocation.azimuth, geolocation.location};
}

Geolocation DevicePositionVsGeolocation::get(
    const core::DevicePosition& devicePosition)
{
    return Geolocation{devicePosition.coordinate, devicePosition.azimuth};
}

} // namespace mpk::dss::backend::rest
