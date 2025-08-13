#pragma once

namespace mpk::dss::core
{

struct DeviceWorkzoneSector
{
    int number;
    double distance;
    double minAngle;
    double maxAngle;
};

bool operator==(
    const DeviceWorkzoneSector& left, const DeviceWorkzoneSector& right);
bool operator!=(
    const DeviceWorkzoneSector& left, const DeviceWorkzoneSector& right);

} // namespace mpk::dss::core
