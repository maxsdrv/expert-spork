#include "DeviceHwInfo.h"

namespace mpk::dss::core
{

bool operator==(const DeviceHwInfo& left, const DeviceHwInfo& right)
{
    return std::tie(left.values) == std::tie(right.values);
}

bool operator!=(const DeviceHwInfo& left, const DeviceHwInfo& right)
{
    return !(left == right);
}

} // namespace mpk::dss::core
