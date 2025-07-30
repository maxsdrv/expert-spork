#pragma once

#include "dss-core/DeviceWorkzoneSector.h"

#include <vector>

namespace mpk::dss::core
{

struct DeviceWorkzone
{
    std::vector<DeviceWorkzoneSector> sectors;
};

bool operator==(const DeviceWorkzone& left, const DeviceWorkzone& right);
bool operator!=(const DeviceWorkzone& left, const DeviceWorkzone& right);

} // namespace mpk::dss::core
