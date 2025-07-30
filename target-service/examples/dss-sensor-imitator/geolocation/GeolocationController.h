#pragma once

#include "dss-backend/rest/geolocation/Geolocation.h"

namespace mpk::dss
{

class GeolocationController
{
public:
    explicit GeolocationController(backend::rest::Geolocation location);

    void setCurrent(const backend::rest::Geolocation& location);
    [[nodiscard]] backend::rest::Geolocation current() const;

private:
    backend::rest::Geolocation m_geolocation;
};

} // namespace mpk::dss
