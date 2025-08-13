#pragma once

#include "OAServiceGeo_position_mode.h"

#include <QGeoCoordinate>

namespace mpk::dss::core
{

struct DevicePosition
{
    double azimuth = 0.0;
    QGeoCoordinate coordinate = {0.0, 0.0, 0.0};
};

bool operator==(const DevicePosition& left, const DevicePosition& right);
bool operator!=(const DevicePosition& left, const DevicePosition& right);

using DevicePositionMode =
    OAService::OAServiceGeo_position_mode::eOAServiceGeo_position_mode;

struct DevicePositionExtended
{
    QString key;
    DevicePositionMode mode;
    DevicePosition position;
};

bool operator==(
    const DevicePositionExtended& left, const DevicePositionExtended& right);
bool operator!=(
    const DevicePositionExtended& left, const DevicePositionExtended& right);

} // namespace mpk::dss::core
