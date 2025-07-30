#pragma once

#include <string>

namespace mpk::dss::backend::rest
{

struct DeviceDiagnosticData
{
    struct Tags
    {
        static constexpr auto ok = "ok";
        static constexpr auto failure = "failure";
    };

    std::string state;
    double cpuTemp{};
};

bool operator==(
    const DeviceDiagnosticData& left, const DeviceDiagnosticData& right);
bool operator!=(
    const DeviceDiagnosticData& left, const DeviceDiagnosticData& right);

} // namespace mpk::dss::backend::rest
