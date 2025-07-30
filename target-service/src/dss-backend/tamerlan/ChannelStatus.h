#pragma once

#include <cstdint>

namespace mpk::dss::backend::tamerlan
{

enum class ChannelStatus : std::uint16_t
{
    NO_ALARM = 0x0000,
    ALARM = 0x0001,
    HARDWARE_ERROR = 0x8000,
    SOFTWARE_ERROR = 0x80F0
};

} // namespace mpk::dss::backend::tamerlan
