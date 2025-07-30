#include "log/DelayTimer.h"

namespace appkit
{

namespace logger
{

namespace
{

constexpr DelayTimer::Duration DefaultDelay = std::chrono::minutes(1);

} // namespace

DelayTimer::DelayTimer() : m_delay(DefaultDelay)
{
}

DelayTimer::DelayTimer(Duration delay) : m_delay(delay)
{
}

bool DelayTimer::check()
{
    auto now = Clock::now();
    bool expired = now - m_lastTime >= m_delay;
    if (expired)
    {
        m_lastTime = now;
    }

    return expired;
}

void DelayTimer::reset()
{
    m_lastTime = TimePoint{};
}

} // namespace logger

} // namespace appkit
