#pragma once

#include "log/Log2.h"

#include "rpc/RequestTracker.h"
#include "rpc/Tags.h"
#include "rpc/Types.h"
#include "rpc/transport/IncomingMessage.h"
#include "rpc/transport/Transport.h"

#include "gsl/assert"

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <memory>
#include <tuple>
#include <unordered_map>

namespace rpc
{

template <typename ProtocolType, typename Timer>
class RpcClient
{
public:
    RpcClient(std::unique_ptr<Transport> transport, ProtocolType&& protocol) :
      m_protocol(std::move(protocol)), m_transport(std::move(transport))
    {
        Expects(m_transport);
        m_transport->onReceived(
            [this](const auto& message) { processResponse(message->buffer()); }, m_tracker());
    }

protected:
    template <typename R, typename... Args>
    std::shared_ptr<RequestTracker<R>> sendRequest(const MethodName& method, Args... args)
    {
        boost::hash<boost::uuids::uuid> uuid_hasher;
        auto id = static_cast<RequestId>(uuid_hasher(boost::uuids::random_generator()()));
        auto requestTracker = std::make_shared<RequestTracker<R>>(id);

        std::shared_ptr<Timer> timer;
        if (m_timeout > 0)
        {
            timer = std::make_shared<Timer>(m_timeout);
            timer->onTriggered(
                [this, requestTracker, id]()
                {
                    if (auto it = m_requests.find(id); it != m_requests.end())
                    {
                        requestTracker->setError(ReturnCode::TIMEOUT, "Request timeout");
                        m_requests.erase(it);
                    }
                },
                m_tracker());
        }

        auto wrapper = [requestTracker, timer](Deserializer& response)
        {
            ReturnCode result{};
            response.unpackValue(rpc::tag::code, result);
            if (result == ReturnCode::SUCCESS)
            {
                if constexpr (std::is_same_v<R, void>)
                {
                    requestTracker->setSuccess();
                }
                else
                {
                    R resultValue;
                    response.unpackValue(rpc::tag::result, resultValue);
                    requestTracker->setSuccess(resultValue);
                }
            }
            else
            {
                std::string errorString;
                response.unpackValue(rpc::tag::error, errorString);
                requestTracker->setError(result, errorString);
            }
        };

        auto buffer = m_protocol.serializer()
                          .packValue(rpc::tag::id, id)
                          .packValue(rpc::tag::method, method)
                          .packArray(rpc::tag::args, args...)
                          .buffer();
        m_requests.emplace(id, std::move(wrapper));
        m_transport->send(buffer);

        return requestTracker;
    }

    void processResponse(typename ProtocolType::BufferType buffer)
    {
        auto response = m_protocol.deserializer(buffer);
        RequestId id = 0;
        response.unpackValue(rpc::tag::id, id);

        if (auto it = m_requests.find(id); it != m_requests.end())
        {
            it->second(response);
            m_requests.erase(it);
        }
        else
        {
            LOG_WARNING << "No request with id " << id;
        }
    }

    void setTimeout(int timeout)
    {
        m_timeout = timeout;
    }

    using Deserializer = typename ProtocolType::DeserializerType;

private:
    ProtocolType m_protocol;

    std::unique_ptr<Transport> m_transport;

    using Wrapper = std::function<void(Deserializer&)>;
    std::unordered_map<RequestId, Wrapper> m_requests;

    int m_timeout = 0;
    boostsignals::SignalTracker m_tracker;
};

template <typename T>
struct RpcMethod;

template <typename R, typename... Args>
struct RpcMethod<R(Args...)>
{
    using ReturnType = R;
    template <size_t I>
    using Arg = typename std::tuple_element<I, std::tuple<Args...>>::type;
    static constexpr size_t ArgsCount = sizeof...(Args);
};

} // namespace rpc

#define RPC_RESULT(...) rpc::RpcMethod<__VA_ARGS__>::ReturnType
#define RPC_ARG(N, ...) rpc::RpcMethod<__VA_ARGS__>::Arg<N>

#define RPC_METHOD0(Name, Timeout, ...)                                    \
    static_assert(                                                         \
        0 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                       \
        "RPC_METHOD<N> must match argument count.");                       \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name()   \
    {                                                                      \
        this->setTimeout(Timeout);                                         \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(#Name); \
    }

#define RPC_METHOD1(Name, Timeout, ...)                                                        \
    static_assert(                                                                             \
        1 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                                           \
        "RPC_METHOD<N> must match argument count.");                                           \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name(RPC_ARG(0, __VA_ARGS__) \
                                                                           arg0)               \
    {                                                                                          \
        this->setTimeout(Timeout);                                                             \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(#Name, arg0);               \
    }

#define RPC_METHOD2(Name, Timeout, ...)                                                \
    static_assert(                                                                     \
        2 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                                   \
        "RPC_METHOD<N> must match argument count.");                                   \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name(                \
        RPC_ARG(0, __VA_ARGS__) arg0, RPC_ARG(1, __VA_ARGS__) arg1)                    \
    {                                                                                  \
        this->setTimeout(Timeout);                                                     \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(#Name, arg0, arg1); \
    }

#define RPC_METHOD3(Name, Timeout, ...)                                                      \
    static_assert(                                                                           \
        3 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                                         \
        "RPC_METHOD<N> must match argument count.");                                         \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name(                      \
        RPC_ARG(0, __VA_ARGS__) arg0,                                                        \
        RPC_ARG(1, __VA_ARGS__) arg1,                                                        \
        RPC_ARG(2, __VA_ARGS__) arg2)                                                        \
    {                                                                                        \
        this->setTimeout(Timeout);                                                           \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(#Name, arg0, arg1, arg2); \
    }

#define RPC_METHOD4(Name, Timeout, ...)                                 \
    static_assert(                                                      \
        4 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                    \
        "RPC_METHOD<N> must match argument count.");                    \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name( \
        RPC_ARG(0, __VA_ARGS__) arg0,                                   \
        RPC_ARG(1, __VA_ARGS__) arg1,                                   \
        RPC_ARG(2, __VA_ARGS__) arg2,                                   \
        RPC_ARG(3, __VA_ARGS__) arg3)                                   \
    {                                                                   \
        this->setTimeout(Timeout);                                      \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(     \
            #Name, arg0, arg1, arg2, arg3);                             \
    }

#define RPC_METHOD5(Name, Timeout, ...)                                 \
    static_assert(                                                      \
        5 == rpc::RpcMethod<__VA_ARGS__>::ArgsCount,                    \
        "RPC_METHOD<N> must match argument count.");                    \
    std::shared_ptr<rpc::RequestTracker<RPC_RESULT(__VA_ARGS__)>> Name( \
        RPC_ARG(0, __VA_ARGS__) arg0,                                   \
        RPC_ARG(1, __VA_ARGS__) arg1,                                   \
        RPC_ARG(2, __VA_ARGS__) arg2,                                   \
        RPC_ARG(3, __VA_ARGS__) arg3,                                   \
        RPC_ARG(4, __VA_ARGS__) arg4)                                   \
    {                                                                   \
        this->setTimeout(Timeout);                                      \
        return this->template sendRequest<RPC_RESULT(__VA_ARGS__)>(     \
            #Name, arg0, arg1, arg2, arg3, arg4);                       \
    }
