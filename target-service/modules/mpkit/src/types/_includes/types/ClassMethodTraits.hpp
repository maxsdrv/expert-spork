#pragma once

#include <tuple>

namespace mpk::types
{

template <typename Method>
struct ClassMethodTraits;

template <typename C, typename R, typename... Args>
struct ClassMethodTraitsBase
{
    using Class = C;
    using ReturnType = R;
    using ArgTypes = std::tuple<Args...>;
    static constexpr std::size_t ArgCount = sizeof...(Args);
    template <std::size_t N>
    using NthArg = std::tuple_element_t<N, ArgTypes>;
};

template <typename C, typename R, typename... Args>
struct ClassMethodTraits<R (C::*)(Args...)> : ClassMethodTraitsBase<C, R, Args...>
{
};

template <typename C, typename R, typename... Args>
struct ClassMethodTraits<R (C::*)(Args...) const> : ClassMethodTraitsBase<C, R, Args...>
{
};

template <typename C, typename R, typename... Args>
struct ClassMethodTraits<R (C::*)(Args...) noexcept> : ClassMethodTraitsBase<C, R, Args...>
{
};

template <typename C, typename R, typename... Args>
struct ClassMethodTraits<R (C::*)(Args...) const noexcept> : ClassMethodTraitsBase<C, R, Args...>
{
};

} // namespace mpk::types
