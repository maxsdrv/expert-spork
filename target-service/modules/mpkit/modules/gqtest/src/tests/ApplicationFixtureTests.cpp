/** @file
 * @brief     Application fixture test suite
 *
 *
 * $Id: $
 */

#include "ApplicationFixture.h"

#include "gqtest"

#include <gmock/gmock.h>

#include <QTimer>

namespace gqtest
{

class Receiver
{
public:
    MOCK_CONST_METHOD0(trigger, void());
};

TEST(ApplicationFixtureTest, testOldStyleConnect)
{
    // Check that QTimer is shot, so event loop is started
    QTimer timer;
    Receiver receiver;
    QObject::connect(&timer, &QTimer::timeout, [&]() { receiver.trigger(); });

    EXPECT_CALL(receiver, trigger()).Times(testing::Exactly(1));
    ApplicationFixture app;
    timer.start(1);
    app.exec(&timer, SIGNAL(timeout()), 1000);
    EXPECT_FALSE(app.isTimedOut());
}

TEST(ApplicationFixtureTest, testNewStyleConnect)
{
    // This test is the same as previous, only new style connect is used is exec
    QTimer timer;
    Receiver receiver;
    QObject::connect(&timer, &QTimer::timeout, [&]() { receiver.trigger(); });

    EXPECT_CALL(receiver, trigger()).Times(testing::Exactly(1));
    ApplicationFixture app;
    timer.start(1);
    app.exec(&timer, &QTimer::timeout, 1000);
    EXPECT_FALSE(app.isTimedOut());
}

TEST(ApplicationFixtureTest, testTimeout)
{
    QTimer timer;
    Receiver receiver;
    QObject::connect(&timer, &QTimer::timeout, [&]() { receiver.trigger(); });

    EXPECT_CALL(receiver, trigger()).Times(testing::Exactly(0));

    ApplicationFixture app;
    timer.start(10000);
    app.exec(&timer, &QTimer::timeout, 1);
    EXPECT_TRUE(app.isTimedOut());
}

} // namespace gqtest
