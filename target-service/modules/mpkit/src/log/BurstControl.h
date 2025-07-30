#pragma once

#include <boost/log/expressions/keyword.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>

#include <chrono>
#include <map>
#include <string>

namespace appkit
{

namespace logger
{

class BurstControl
{
public:
    using Clock = std::chrono::steady_clock;
    explicit BurstControl(Clock::duration delay);

    bool verify(const std::string& channel);

private:
    const Clock::duration m_delay;
    std::map<std::string, std::chrono::time_point<Clock>> m_channels;
};

} // namespace logger

} // namespace appkit
