#pragma once

#include <cstdint>
#include <ostream>

namespace mpk::drone::core
{

enum class ChannelStatus : std::uint16_t
{
    NO_ALARM = 0x0000,
    ALARM = 0x0001,
    SOFTWARE_ERROR = 0x80F0
};

std::ostream& operator<<(std::ostream& stream, ChannelStatus status);

} // namespace mpk::drone::core
