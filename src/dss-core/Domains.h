#pragma once

namespace mpk::dss::core
{

struct Domain
{
    static constexpr auto Sensor = "sensor";
    static constexpr auto Jammer = "jammer";
    static constexpr auto Camera = "camera";
    static constexpr auto Target = "target";
    static constexpr auto General = "general";
};

} // namespace mpk::dss::core
