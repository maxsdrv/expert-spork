#include "TransportMock.h"

#include "rpc/MultiRequestTracker.h"
#include "rpc/RpcClient.h"
#include "rpc/SimpleTimer.h"
#include "rpc/proto/JsonProtocol.h"

#include "ApplicationFixture.h"
#include "gqtest"

#include <gmock/gmock.h>

#include <functional>
#include <memory>
#include <thread>

namespace rpc
{

template <typename T>
class CompleteHandler
{
public:
    MOCK_CONST_METHOD0(complete, void());
    MOCK_CONST_METHOD1_T(success, void(T));
    MOCK_CONST_METHOD2(error, void(ReturnCode, std::string));
};

template <>
class CompleteHandler<void>
{
public:
    MOCK_CONST_METHOD0(complete, void());
    MOCK_CONST_METHOD0(success, void());
    MOCK_CONST_METHOD2(error, void(ReturnCode, std::string));
};

class SimpleRpcClient : public RpcClient<Protocol, SimpleTimer>
{
public:
    explicit SimpleRpcClient(std::unique_ptr<Transport> transport) :
      RpcClient<Protocol, SimpleTimer>(
          std::move(transport), Protocol(DataFormat::Json))
    {
    }

    RPC_METHOD0(noArgs, 100, std::string())
    RPC_METHOD1(returnVoid, 100, void(int16_t))
    RPC_METHOD2(addTwoInts, 100, int16_t(int16_t, int16_t))
};

class RpcClientTest : public ::testing::Test

{
public:
    RpcClientTest() :
      m_transport(new TransportMock()),
      m_client(std::unique_ptr<TransportMock>(m_transport)),
      m_protocol(DataFormat::Json)
    {
    }

    template <typename R, typename... Args>
    QByteArray checkRequestAndPackResponse(
        QByteArray const& request,
        std::string const& methodName,
        R result,
        Args... args)
    {
        RequestId id;
        MethodName method;
        std::tuple<Args...> _args;

        m_protocol.deserializer(request)
            .unpackValue(tag::id, id)
            .unpackValue(tag::method, method)
            .unpackArray(tag::args, _args);

        EXPECT_EQ(methodName, method);
        EXPECT_EQ(std::make_tuple(args...), _args);

        return m_protocol.serializer()
            .packValue(tag::id, id)
            .packValue(tag::code, ReturnCode::SUCCESS)
            .packValue(tag::result, result)
            .buffer();
    }

protected:
    TransportMock* m_transport;
    SimpleRpcClient m_client;

    Protocol m_protocol;
    boostsignals::SignalTracker m_tracker;
};

TEST_F(RpcClientTest, callWithTwoArgsSuccessComplete)
{
    auto requestTracker = m_client.addTwoInts(10, 20);

    CompleteHandler<int16_t> completeHandler;
    requestTracker->onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    requestTracker->onError(
        [&completeHandler](auto returnCode, auto errorMessage) {
            completeHandler.error(returnCode, errorMessage);
        },
        m_tracker());

    requestTracker->onSuccess(
        [&completeHandler](auto result) { completeHandler.success(result); },
        m_tracker());

    const int16_t callResult{30};
    EXPECT_CALL(completeHandler, success(callResult)).Times(testing::Exactly(1));
    EXPECT_CALL(completeHandler, error(testing::_, testing::_))
        .Times(testing::Exactly(0));
    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    QByteArray response = checkRequestAndPackResponse(
        m_transport->buffer(),
        "addTwoInts",
        callResult,
        int16_t{10},
        int16_t{20});

    m_transport->receive(response);
}

TEST_F(RpcClientTest, callWithNoArgsSuccessComplete)
{
    auto requestTracker = m_client.noArgs();

    CompleteHandler<std::string> completeHandler;
    requestTracker->onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    requestTracker->onError(
        [&completeHandler](auto returnCode, auto errorMessage) {
            completeHandler.error(returnCode, errorMessage);
        },
        m_tracker());

    requestTracker->onSuccess(
        [&completeHandler](auto result) { completeHandler.success(result); },
        m_tracker());

    const std::string callResult{"hello"};
    EXPECT_CALL(completeHandler, success(callResult)).Times(testing::Exactly(1));
    EXPECT_CALL(completeHandler, error(testing::_, testing::_))
        .Times(testing::Exactly(0));
    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    QByteArray response =
        checkRequestAndPackResponse(m_transport->buffer(), "noArgs", callResult);

    m_transport->receive(response);
}

TEST_F(RpcClientTest, callReturnsVoid)
{
    auto requestTracker = m_client.returnVoid(18);

    CompleteHandler<void> completeHandler;
    requestTracker->onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    requestTracker->onError(
        [&completeHandler](auto returnCode, auto errorMessage) {
            completeHandler.error(returnCode, errorMessage);
        },
        m_tracker());

    requestTracker->onSuccess(
        [&completeHandler]() { completeHandler.success(); }, m_tracker());

    EXPECT_CALL(completeHandler, success()).Times(testing::Exactly(1));
    EXPECT_CALL(completeHandler, error(testing::_, testing::_))
        .Times(testing::Exactly(0));
    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    RequestId id;
    MethodName method;
    std::tuple<int16_t> args;

    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(tag::id, id)
        .unpackValue(tag::method, method)
        .unpackArray(tag::args, args);

    EXPECT_EQ("returnVoid", method);
    EXPECT_EQ(18, std::get<0>(args));

    auto response = m_protocol.serializer()
                        .packValue(tag::id, id)
                        .packValue(tag::code, ReturnCode::SUCCESS)
                        .buffer();
    m_transport->receive(response);
}

TEST_F(RpcClientTest, callWithTwoArgsFailed)
{
    auto requestTracker = m_client.addTwoInts(10, 20);

    CompleteHandler<int16_t> completeHandler;
    requestTracker->onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    requestTracker->onError(
        [&completeHandler](auto returnCode, auto errorMessage) {
            completeHandler.error(returnCode, errorMessage);
        },
        m_tracker());

    requestTracker->onSuccess(
        [&completeHandler](auto result) { completeHandler.success(result); },
        m_tracker());

    const std::string errorMessage{"error"};
    EXPECT_CALL(completeHandler, success(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(
        completeHandler, error(ReturnCode::INVALID_METHOD, errorMessage))
        .Times(testing::Exactly(1));
    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    RequestId id;
    MethodName method;
    std::tuple<int16_t, int16_t> args;

    m_protocol.deserializer(m_transport->buffer())
        .unpackValue(tag::id, id)
        .unpackValue(tag::method, method)
        .unpackArray(tag::args, args);

    EXPECT_EQ("addTwoInts", method);
    EXPECT_EQ(10, std::get<0>(args));
    EXPECT_EQ(20, std::get<1>(args));

    auto response =
        m_protocol.serializer()
            .packValue(tag::id, id)
            .packValue(tag::code, ReturnCode::INVALID_METHOD)
            .packValue(tag::error, errorMessage)
            .buffer();
    m_transport->receive(response);
}

TEST_F(RpcClientTest, callWithTwoArgsTimeout)
{
    gqtest::ApplicationFixture app;

    auto requestTracker = m_client.addTwoInts(10, 20);

    CompleteHandler<int16_t> completeHandler;
    requestTracker->onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    requestTracker->onError(
        [&completeHandler](auto returnCode, auto errorMessage) {
            completeHandler.error(returnCode, errorMessage);
        },
        m_tracker());

    requestTracker->onSuccess(
        [&completeHandler](auto result) { completeHandler.success(result); },
        m_tracker());

    const std::string errorMessage{"Request timeout"};
    EXPECT_CALL(completeHandler, success(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(completeHandler, error(ReturnCode::TIMEOUT, errorMessage))
        .Times(testing::Exactly(1));
    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    app.wait(200);
}

TEST_F(RpcClientTest, multiTrackerWithTwoSuccessRequests)
{
    auto request1 = m_client.addTwoInts(10, 20);
    QByteArray response1 = checkRequestAndPackResponse(
        m_transport->buffer(),
        "addTwoInts",
        int16_t{30},
        int16_t{10},
        int16_t{20});

    auto request2 = m_client.addTwoInts(1, 2);
    QByteArray response2 = checkRequestAndPackResponse(
        m_transport->buffer(), "addTwoInts", int16_t{3}, int16_t{1}, int16_t{2});

    MultiRequestTracker multiTracker;
    multiTracker.track("request1", request1);
    multiTracker.track("request2", request2);

    CompleteHandler<void> completeHandler;
    multiTracker.onComplete(
        [&completeHandler]() { completeHandler.complete(); }, m_tracker());

    EXPECT_CALL(completeHandler, complete()).Times(testing::Exactly(1));

    m_transport->receive(response2);
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    m_transport->receive(response1);

    EXPECT_EQ(30, multiTracker.tracker<int16_t>("request1")->result());
    EXPECT_EQ(3, multiTracker.tracker<int16_t>("request2")->result());
}

TEST_F(RpcClientTest, multiTrackerLifetimeTest)
{
    std::weak_ptr<RequestTracker<int16_t>> requestWptr;
    std::weak_ptr<MultiRequestTracker> multiTrackerWptr;

    {
        auto request = m_client.addTwoInts(10, 20);
        requestWptr = request;
        auto multiTracker = std::make_shared<MultiRequestTracker>();
        multiTrackerWptr = multiTracker;
        multiTracker->track("request", request);
        multiTracker->onComplete(
            [multiTracker]() mutable { multiTracker.reset(); }, m_tracker());
    }

    EXPECT_FALSE(requestWptr.expired());
    EXPECT_FALSE(multiTrackerWptr.expired());

    QByteArray response = checkRequestAndPackResponse(
        m_transport->buffer(),
        "addTwoInts",
        int16_t{30},
        int16_t{10},
        int16_t{20});

    m_transport->receive(response);

    EXPECT_TRUE(requestWptr.expired());
    EXPECT_TRUE(multiTrackerWptr.expired());
}

} // namespace rpc

