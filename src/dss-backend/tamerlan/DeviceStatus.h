#pragma once

#include <cstdint>

namespace mpk::dss::backend::tamerlan
{

enum class DeviceStatus : std::uint16_t
{
    NORMAL_MODE = 0x0000,
    SERVICE_MODE = 0x0001,
    SIGNAL_DEVICE_ERROR = 0x8000,
    EXTERNAL_DEVICE_ERROR = 0x8010,
    LICENSE_ERROR = 0x8020,
    SOFTWARE_ERROR = 0x80F0
};

} // namespace mpk::dss::backend::tamerlan
