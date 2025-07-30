#pragma once

#include <cinttypes>
#include <type_traits>

namespace concepts
{

template <typename T>
constexpr bool IsBigInt = std::is_integral_v<T> && sizeof(T) == sizeof(int64_t);

template <typename T>
constexpr bool IsSignedBigInt = IsBigInt<T>&& std::is_signed_v<T>;

template <typename T>
constexpr bool IsUnsignedBigInt = IsBigInt<T>&& std::is_unsigned_v<T>;

template <typename T>
constexpr bool IsSmallInt = std::is_integral_v<T> && sizeof(T) < sizeof(int64_t);

} // namespace concepts
