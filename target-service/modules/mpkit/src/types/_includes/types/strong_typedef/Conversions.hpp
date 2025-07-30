#pragma once

#include "Basic.hpp"

namespace mpk
{

namespace types
{

/// Adds put to arbitrary stream operator
template <typename StrongType, typename StreamType>
class Streamable
{
public:
    friend StreamType operator<<(StreamType stream, StrongType const& value)
    {
        stream << weak_cast(value);
        return stream;
    }
};

template <typename To>
struct StaticCastConverter
{
    template <typename From>
    static To convert(From const& value)
    {
        return static_cast<To>(value);
    };
};

/// Adds conversion to type T
template <typename StrongType, typename T, typename Convertor = StaticCastConverter<T>>
class ConvertibleTo
{
public:
    constexpr explicit operator T() const noexcept
    {
        auto const& self = *static_cast<StrongType const*>(this);
        return Convertor::convert(weak_cast(self));
    }
};

} // namespace types

} // namespace mpk
