#include "GeolocationController.h"

#include <utility>

namespace mpk::dss
{

GeolocationController::GeolocationController(
    backend::rest::Geolocation location) :
  m_geolocation(std::move(location))
{
}

void GeolocationController::setCurrent(
    const backend::rest::Geolocation& location)
{
    m_geolocation = location;
}

backend::rest::Geolocation GeolocationController::current() const
{
    return m_geolocation;
}

} // namespace mpk::dss
