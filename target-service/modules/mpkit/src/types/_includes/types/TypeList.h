#pragma once

namespace typelist
{

struct Nil
{
};

template <class H, class T = Nil>
struct Cons
{
};

template <class L>
struct Head
{
    using type = L;
};

template <class H, class T>
struct Head<Cons<H, T>>
{
    using type = H;
};

template <class L>
struct Tail
{
    using type = Nil;
};

template <class H, class T>
struct Tail<Cons<H, T>>
{
    using type = T;
};

template <class L>
using HeadT = typename Head<L>::type;

template <class L>
using TailT = typename Tail<L>::type;

template <class... Types>
struct List
{
};

template <>
struct List<>
{
    using type = Nil;
};

template <class H, class... T>
struct List<H, T...>
{
    using type = Cons<H, typename List<T...>::type>;
};

template <class... Types>
using ListT = typename List<Types...>::type;

} // namespace typelist
