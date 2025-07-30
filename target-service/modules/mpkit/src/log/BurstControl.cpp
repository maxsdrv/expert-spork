#include "BurstControl.h"

#include <boost/log/expressions.hpp>
#include <boost/log/keywords/delimiter.hpp>

namespace appkit
{

namespace logger
{

BurstControl::BurstControl(Clock::duration delay) : m_delay(delay)
{
}

bool BurstControl::verify(const std::string& channel)
{
    auto now = Clock::now();
    auto [it, inserted] = m_channels.insert({channel, now});
    if (inserted)
    {
        return true;
    }
    if (now - it->second >= m_delay)
    {
        it->second = now;
        return true;
    }

    return false;
}

} // namespace logger

} // namespace appkit
