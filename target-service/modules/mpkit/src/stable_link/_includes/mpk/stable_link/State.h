#pragma once

#include <cstdint>

namespace mpk::stable_link
{

enum class State : std::uint8_t
{
    OFFLINE,
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

} // namespace mpk::stable_link
