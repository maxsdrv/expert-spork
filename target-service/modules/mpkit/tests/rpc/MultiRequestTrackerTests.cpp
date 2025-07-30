#include "boost/SignalTracker.h"

#include "rpc/MultiRequestTracker.h"

#include "ApplicationFixture.h"

#include <gmock/gmock.h>

namespace rpc
{

template <typename ReturnType>
class RequestTrackerTypedMock: public RequestTracker<ReturnType>
{
public:
    RequestTrackerTypedMock(): RequestTracker<ReturnType>(RequestId{})
    {}

    void setSuccess(const ReturnType& value)
    {
        RequestTracker<ReturnType>::setSuccess(value);
    }
};

class MultiTrackerHandler
{
public:
    MOCK_CONST_METHOD0(complete, void());
};

class MultiRequestTrackerTest : public ::testing::Test
{
public:
    void SetUp() final
    {
        m_tracker.onComplete([this](){ m_handler.complete(); }, m_guard());
    }

protected:
    MultiRequestTracker m_tracker;
    MultiTrackerHandler m_handler;
    boostsignals::SignalTracker m_guard;
};

TEST_F(MultiRequestTrackerTest, NoRequests)
{
    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(0));
    EXPECT_TRUE(m_tracker.succeeded());
}

TEST_F(MultiRequestTrackerTest, SingleRequestNotCompleted)
{
    auto tracker = std::make_shared<RequestTrackerTypedMock<int>>();
    m_tracker.track("id", tracker);

    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(0));
    EXPECT_FALSE(m_tracker.succeeded());
}

TEST_F(MultiRequestTrackerTest, SingleRequestCompleted)
{
    auto tracker = std::make_shared<RequestTrackerTypedMock<int>>();
    m_tracker.track("id", tracker);

    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(1));

    tracker->setSuccess(42);

    EXPECT_TRUE(m_tracker.succeeded());
}

TEST_F(MultiRequestTrackerTest, MultiRequestsNotCompleted)
{
    auto tracker1 = std::make_shared<RequestTrackerTypedMock<int>>();
    m_tracker.track("id1", tracker1);
    auto tracker2 = std::make_shared<RequestTrackerTypedMock<std::string>>();
    m_tracker.track("id2", tracker2);

    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(0));
    EXPECT_FALSE(m_tracker.succeeded());
}

TEST_F(MultiRequestTrackerTest, MultiRequestsPartiallyCompleted)
{
    auto tracker1 = std::make_shared<RequestTrackerTypedMock<int>>();
    m_tracker.track("id1", tracker1);
    auto tracker2 = std::make_shared<RequestTrackerTypedMock<std::string>>();
    m_tracker.track("id2", tracker2);

    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(0));

    tracker1->setSuccess(42);

    EXPECT_FALSE(m_tracker.succeeded());
}

TEST_F(MultiRequestTrackerTest, MultiRequestsCompleted)
{
    auto tracker1 = std::make_shared<RequestTrackerTypedMock<int>>();
    m_tracker.track("id1", tracker1);
    auto tracker2 = std::make_shared<RequestTrackerTypedMock<std::string>>();
    m_tracker.track("id2", tracker2);

    EXPECT_CALL(m_handler, complete()).Times(testing::Exactly(1));

    tracker1->setSuccess(42);
    tracker2->setSuccess("Hooray");

    EXPECT_TRUE(m_tracker.succeeded());
}

} // rpc
