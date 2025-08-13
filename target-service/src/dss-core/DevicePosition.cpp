#include "DevicePosition.h"

namespace mpk::dss::core
{

bool operator==(const DevicePosition& left, const DevicePosition& right)
{
    return std::tie(left.azimuth, left.coordinate)
           == std::tie(right.azimuth, right.coordinate);
}

bool operator!=(const DevicePosition& left, const DevicePosition& right)
{
    return !(left == right);
}

bool operator==(
    const DevicePositionExtended& left, const DevicePositionExtended& right)
{
    return std::tie(left.key, left.mode, left.position)
           == std::tie(right.key, right.mode, right.position);
}

bool operator!=(
    const DevicePositionExtended& left, const DevicePositionExtended& right)
{
    return !(left == right);
}

} // namespace mpk::dss::core
