#include "DeviceDiagnosticData.h"

#include <tuple>

namespace mpk::dss::backend::rest
{

bool operator==(
    const DeviceDiagnosticData& left, const DeviceDiagnosticData& right)
{
    return std::tie(left.state, left.cpuTemp)
           == std::tie(right.state, right.cpuTemp);
}

bool operator!=(
    const DeviceDiagnosticData& left, const DeviceDiagnosticData& right)
{
    return !(left == right);
}

} // namespace mpk::dss::backend::rest
