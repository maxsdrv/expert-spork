#pragma once

namespace mpk::stable_link
{

struct StateTag
{
    static constexpr auto offline = "Offline";
    static constexpr auto disconnected = "Disconnected";
    static constexpr auto connecting = "Connecting";
    static constexpr auto connected = "Connected";
};

} // namespace mpk::stable_link
