#pragma once

#include "rpc/Tags.h"
#include "rpc/Types.h"

#include "exception/DuplicateEntity.h"
#include "exception/MissingEntity.h"

#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace rpc
{

template <typename>
struct FunctionMetatype;

template <typename R, typename... T>
struct FunctionMetatype<std::function<R(T...)>>
{
    using ArgumentsType = std::tuple<std::decay_t<T>...>;
    using ReturnType = std::decay_t<R>;
};

template <typename ProtocolType>
class RpcHandlers
{
public:
    using Serializer = typename ProtocolType::SerializerType;
    using Deserializer = typename ProtocolType::DeserializerType;
    using Wrapper = std::function<void(Deserializer&, Serializer&)>;

    const Wrapper& get(const MethodName& method) const
    {
        auto it = m_handlers.find(method);
        if (it == m_handlers.end())
        {
            BOOST_THROW_EXCEPTION(exception::MissingEntity());
        }
        return it->second;
    }

    template <typename Handler>
    void add(MethodName method, Handler func)
    {
        if (m_handlers.find(method) != m_handlers.end())
        {
            BOOST_THROW_EXCEPTION(exception::DuplicateEntity());
        }

        auto wrapper = [f = std::move(func)](Deserializer& request, Serializer& response)
        {
            std::function func{std::move(f)};
            using metatype = FunctionMetatype<decltype(func)>;
            using ArgumentsType = typename metatype::ArgumentsType;
            using ReturnType = typename metatype::ReturnType;

            ArgumentsType args;
            request.unpackArray(rpc::tag::args, args);

            if constexpr (std::is_same_v<ReturnType, void>)
            {
                std::apply(std::move(func), std::move(args));
                response.packValue(rpc::tag::code, ReturnCode::SUCCESS);
            }
            else
            {
                auto result = std::apply(std::move(func), std::move(args));
                response.packValue(rpc::tag::code, ReturnCode::SUCCESS)
                    .packValue(rpc::tag::result, result);
            }
        };

        m_handlers.emplace(std::move(method), std::move(wrapper));
    }

private:
    std::unordered_map<MethodName, Wrapper> m_handlers;
};

} // namespace rpc
