#pragma once

#include <QGeoCoordinate>

#include <iostream>

namespace mpk::dss::backend::rest
{

struct Geolocation
{
    QGeoCoordinate location = {0, 0, 0};
    double azimuth = 0;
};

} // namespace mpk::dss::backend::rest

[[maybe_unused]] std::ostream& operator<<(
    std::ostream& os, QGeoCoordinate const& value);

[[maybe_unused]] std::istream& operator>>(
    std::istream& is, QGeoCoordinate& into);
