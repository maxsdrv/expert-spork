#include "log/DelayTimer.h"
#include "log/Log2.h"

#include "appkit/Paths.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace tests
{

class LogDelayTest : public ::testing::Test
{
public:
    LogDelayTest() : m_log(appkit::configFile("console.log.ini"))
    {
    }

protected:
    appkit::logger::Log2 m_log;
};

void logWithDefaultDelay(unsigned int& logCount)
{
    static appkit::logger::DelayTimer timer;
    if (timer.check())
    {
        LOG_TRACE << "Default delay test";
        ++logCount;
    }
}

void logWithCustomDelay(unsigned int& logCount)
{
    static appkit::logger::DelayTimer timer(std::chrono::milliseconds(200));
    if (timer.check())
    {
        LOG_TRACE << "Custom delay test";
        ++logCount;
    }
}

void logWithResetDelay(unsigned int& logCount)
{
    static appkit::logger::DelayTimer timer(std::chrono::milliseconds(200));
    if (timer.check())
    {
        LOG_TRACE << "Reset delay test";
        ++logCount;
    }
    timer.reset();
}

TEST_F(LogDelayTest, defaultDelay)
{
    auto logCount = 0u;
    for (auto i = 0u; i < 3; ++i)
    {
        logWithDefaultDelay(logCount);
    }
    EXPECT_EQ(logCount, 1);
}

TEST_F(LogDelayTest, customDelay)
{
    auto logCount = 0u;
    for (auto i = 0u; i < 4; ++i)
    {
        logWithCustomDelay(logCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_EQ(logCount, 2);
}

TEST_F(LogDelayTest, resetDelay)
{
    auto logCount = 0u;
    for (auto i = 0u; i < 4; ++i)
    {
        logWithResetDelay(logCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    EXPECT_EQ(logCount, 4);
}

} // namespace tests
