#include "boost/SignalTracker.h"

#include "rpc/SimpleTimer.h"

#include "ApplicationFixture.h"

#include <gmock/gmock.h>

namespace rpc
{

class TimerHandler
{
public:
    MOCK_CONST_METHOD0(triggered, void());
};

constexpr auto timeout = 100;

class SimpleTimerTest : public ::testing::Test
{

protected:
    boostsignals::SignalTracker m_tracker;
};

TEST_F(SimpleTimerTest, checkTriggeredAfter)
{
    gqtest::ApplicationFixture app;
    TimerHandler handler;

    SimpleTimer timer(timeout);
    timer.onTriggered([&handler](){handler.triggered();}, m_tracker());

    EXPECT_CALL(handler, triggered()).Times(testing::Exactly(1));

    app.wait(timeout + 20);
}

} // rpc
