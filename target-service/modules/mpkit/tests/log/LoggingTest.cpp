#include "log/Log2.h"

#include "appkit/Paths.h"

#include <gtest/gtest.h>

#include <boost/log/sources/record_ostream.hpp>

#include <chrono>
#include <thread>

namespace tests
{

class LogTest : public ::testing::Test
{
public:
    LogTest()
        : m_log(appkit::configFile("console.log.ini"))
    {
    }

protected:
    appkit::logger::Log2 m_log;
};

TEST_F(LogTest, logDebug)
{
    LOG_TRACE << "Trace log message"; // Should be filtered
    LOG_DEBUG << "Debug log message";
    LOG_INFO << "Info log message";
    LOG_DEBUG_CH("Channel1") << "Test debug channel1 message";
    LOG_DEBUG_CH("Channel1") << "Test another debug channel1 message"; // Should be filtered
    LOG_DEBUG_CH("Channel2") << "Test debug channel2 message";
    LOG_INFO_CH("Channel1") << "Test info channel1 message"; // Still should be filtered
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LOG_INFO_CH("Channel1") << "Test another info channel1 message";
}

} // namespace tests

