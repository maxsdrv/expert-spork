#pragma once

#include "json/JsonComposer.h"

#include <string>
#include <vector>

namespace mpk::dss::sensors
{

struct RfdTargetAttributes
{
    bool digital;
    std::vector<int> frequencies;
    std::string description;
    bool geolocation{false};
};

} // namespace mpk::dss::sensors

template <>
struct ToJsonConverter<mpk::dss::sensors::RfdTargetAttributes>
{
    static QJsonValue convert(const mpk::dss::sensors::RfdTargetAttributes&);
};
