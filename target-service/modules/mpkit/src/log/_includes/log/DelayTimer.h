#pragma once

#include "mpklog_export.h"

#include <chrono>

namespace appkit::logger
{

class MPKLOG_EXPORT DelayTimer
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

    DelayTimer();
    explicit DelayTimer(Duration delay);

    bool check();
    void reset();

private:
    TimePoint m_lastTime;
    const Duration m_delay;
};

} // namespace appkit::logger
