#include "DeviceWorkzone.h"

namespace mpk::dss::core
{

bool operator==(const DeviceWorkzone& left, const DeviceWorkzone& right)
{
    return left.sectors == right.sectors;
}

bool operator!=(const DeviceWorkzone& left, const DeviceWorkzone& right)
{
    return !(left == right);
}

} // namespace mpk::dss::core
