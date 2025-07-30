#pragma once

#include <array>
#include <type_traits>

namespace concepts
{

template <typename A>
struct IsStdArray : std::false_type
{
};

template <typename A, std::size_t N>
struct IsStdArray<std::array<A, N>> : std::true_type
{
};

template <typename T>
inline constexpr bool IsStdArrayV = IsStdArray<T>::value;

} // namespace concepts
