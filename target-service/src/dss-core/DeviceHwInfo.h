#pragma once

#include <QString>

#include <map>

namespace mpk::dss::core
{

struct DeviceHwInfo
{
    struct Tags
    {
        static constexpr auto temperature = "temperature";
        static constexpr auto voltage = "voltage";
        static constexpr auto current = "current";
    };

    std::map<QString, QString> values;
};

bool operator==(const DeviceHwInfo& left, const DeviceHwInfo& right);
bool operator!=(const DeviceHwInfo& left, const DeviceHwInfo& right);

} // namespace mpk::dss::core
