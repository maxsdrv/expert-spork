#pragma once

#include "dss-core/DeviceId.h"
#include "dss-core/JammerBandOption.h"
#include "dss-core/JammerMode.h"

#include <chrono>

namespace mpk::dss::core
{

using namespace std::chrono;

struct JammerTimeoutStatus
{
    system_clock::time_point started;
    system_clock::time_point now;
    int timeoutSec{0};
    JammerBandOption bands;
};

} // namespace mpk::dss::core
