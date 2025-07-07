#include "DeviceWorkzoneSector.h"

#include <tuple>

namespace mpk::dss::core
{

bool operator==(
    const DeviceWorkzoneSector& left, const DeviceWorkzoneSector& right)
{
    return std::tie(left.number, left.distance, left.minAngle, left.maxAngle)
           == std::tie(
               right.number, right.distance, right.minAngle, right.maxAngle);
}

bool operator!=(
    const DeviceWorkzoneSector& left, const DeviceWorkzoneSector& right)
{
    return !(left == right);
}

} // namespace mpk::dss::core
