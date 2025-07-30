#pragma once

#include "rpc/RpcHandlers.h"
#include "rpc/transport/IncomingMessage.h"
#include "rpc/transport/Transport.h"

#include "boost/SignalTracker.h"
#include "json/JsonExceptions.h"
#include "log/Log2.h"

#include "gsl/assert"
#include "gsl/pointers"

#include <memory>

namespace rpc
{

template <typename ProtocolType>
class RpcServer
{
public:
    explicit RpcServer(
        std::unique_ptr<Transport> transport,
        gsl::not_null<const RpcHandlers<ProtocolType>*> handlers,
        const ProtocolType& protocol) :
      m_protocol(protocol), m_transport(std::move(transport)), m_handlers(handlers)
    {
        Expects(m_transport);
        m_transport->onReceived(
            [this](const auto& message) { this->processIncomingRequest(message); }, m_tracker());
    }

    void setHandlers(gsl::not_null<const RpcHandlers<ProtocolType>*> handlers)
    {
        m_handlers = handlers;
    }

private:
    template <typename MessageType>
    void processIncomingRequest(const MessageType& message)
    {
        auto request = m_protocol.deserializer(message->buffer());
        RequestId id{};
        MethodName method;
        request.unpackValue(rpc::tag::id, id).unpackValue(rpc::tag::method, method);

        auto response = m_protocol.serializer();
        response.packValue(rpc::tag::id, id);

        LOG_DEBUG << "Incoming request: " << method;
        try
        {
            auto func = m_handlers->get(method);
            try
            {
                func(request, response);
                LOG_DEBUG << "Method complete: " << method;
            }
            catch (const exception::json::InvalidJson&)
            {
                LOG_WARNING << "Invalid arguments for method: " << method;
                response.packValue(rpc::tag::code, ReturnCode::INVALID_ARGS)
                    .packValue(rpc::tag::error, method);
            }
            catch (const std::exception& ex)
            {
                LOG_WARNING << "Internal error while executing the method: " << ex.what();
                const auto* info = boost::get_error_info<::exception::ExceptionInfo>(ex);
                response.packValue(rpc::tag::code, ReturnCode::INTERNAL_ERROR)
                    .packValue(rpc::tag::error, info ? *info : ex.what());
            }
        }
        catch (const exception::MissingEntity&)
        {
            LOG_WARNING << "Method not registered: " << method;
            response.packValue(rpc::tag::code, ReturnCode::INVALID_METHOD)
                .packValue(rpc::tag::error, method);
        }

        try
        {
            message->reply(response.buffer());
        }
        catch (const std::exception& ex)
        {
            LOG_WARNING << "Internal error while replying to message: " << ex.what();
        }
    }

    ProtocolType m_protocol;
    std::unique_ptr<Transport> m_transport;
    gsl::not_null<const RpcHandlers<ProtocolType>*> m_handlers;
    boostsignals::SignalTracker m_tracker;
};

} // namespace rpc
