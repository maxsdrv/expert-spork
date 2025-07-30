#include "boost/SignalTracker.h"

#include "rpc/RequestTracker.h"

#include "ApplicationFixture.h"

#include <gmock/gmock.h>

namespace rpc
{

class RequestTrackerBaseMock : public RequestTrackerBase
{
public:
    RequestTrackerBaseMock() : RequestTrackerBase(RequestId{})
    {
    }

    void setError(ReturnCode code, const std::string& errorString)
    {
        RequestTrackerBase::setError(code, errorString);
    }
};

template <typename ReturnType>
class RequestTrackerTypedMock : public RequestTracker<ReturnType>
{
public:
    RequestTrackerTypedMock() : RequestTracker<ReturnType>(RequestId{})
    {
    }

    void setSuccess(const ReturnType& value)
    {
        RequestTracker<ReturnType>::setSuccess(value);
    }
};

template <>
class RequestTrackerTypedMock<void> : public RequestTracker<void>
{
public:
    RequestTrackerTypedMock() : RequestTracker<void>(RequestId{})
    {
    }

    void setSuccess()
    {
        RequestTracker<void>::setSuccess();
    }
};

class BaseTrackerHandler
{
public:
    MOCK_CONST_METHOD0(complete, void());
    MOCK_CONST_METHOD2(error, void(ReturnCode, std::string));
};

template <typename T>
class TypedTrackerHandler
{
public:
    MOCK_CONST_METHOD0(complete, void());
    MOCK_CONST_METHOD1(success, void(T));
    MOCK_CONST_METHOD2(error, void(ReturnCode, std::string));
};

template <>
class TypedTrackerHandler<void>
{
public:
    MOCK_CONST_METHOD0(complete, void());
    MOCK_CONST_METHOD0(success, void());
    MOCK_CONST_METHOD2(error, void(ReturnCode, std::string));
};

using ::testing::_;
using ::testing::Eq;

TEST(RequestTrackerTest, BaseTrackerNotComplete)
{
    RequestTrackerBaseMock tracker;
    EXPECT_FALSE(tracker.succeeded());
    EXPECT_EQ(ReturnCode::UNDEFINED, tracker.code());
}

TEST(RequestTrackerTest, BaseTrackerError)
{
    RequestTrackerBaseMock tracker;
    BaseTrackerHandler handler;
    boostsignals::SignalTracker m_guard;
    tracker.onComplete([&handler]() { handler.complete(); }, m_guard());
    tracker.onError([&handler](auto code, auto error) { handler.error(code, error); }, m_guard());

    auto returnCode = ReturnCode::INVALID_ARGS;
    auto errorStr = "No more puppies!";

    EXPECT_CALL(handler, complete()).Times(testing::Exactly(1));
    EXPECT_CALL(handler, error(Eq(returnCode), Eq(errorStr))).Times(testing::Exactly(1));

    tracker.setError(returnCode, errorStr);
    EXPECT_FALSE(tracker.succeeded());
    EXPECT_EQ(returnCode, tracker.code());
    EXPECT_EQ(errorStr, tracker.errorString());
}

TEST(RequestTrackerTest, TypedTrackerSuccess)
{
    RequestTrackerTypedMock<int> tracker;
    TypedTrackerHandler<int> handler;
    boostsignals::SignalTracker m_guard;
    tracker.onComplete([&handler]() { handler.complete(); }, m_guard());
    tracker.onSuccess([&handler](auto value) { handler.success(value); }, m_guard());

    auto value = 42;

    EXPECT_CALL(handler, complete()).Times(testing::Exactly(1));
    EXPECT_CALL(handler, error(_, _)).Times(testing::Exactly(0));
    EXPECT_CALL(handler, success(Eq(value))).Times(testing::Exactly(1));

    tracker.setSuccess(value);
    EXPECT_TRUE(tracker.succeeded());
    EXPECT_EQ(ReturnCode::SUCCESS, tracker.code());
    EXPECT_EQ(value, tracker.result());
}

TEST(RequestTrackerTest, VoidTrackerSuccess)
{
    RequestTrackerTypedMock<void> tracker;
    TypedTrackerHandler<void> handler;
    boostsignals::SignalTracker m_guard;
    tracker.onComplete([&handler]() { handler.complete(); }, m_guard());
    tracker.onSuccess([&handler]() { handler.success(); }, m_guard());

    EXPECT_CALL(handler, complete()).Times(testing::Exactly(1));
    EXPECT_CALL(handler, error(_, _)).Times(testing::Exactly(0));
    EXPECT_CALL(handler, success()).Times(testing::Exactly(1));

    tracker.setSuccess();
    EXPECT_TRUE(tracker.succeeded());
    EXPECT_EQ(ReturnCode::SUCCESS, tracker.code());
}

} // namespace rpc
