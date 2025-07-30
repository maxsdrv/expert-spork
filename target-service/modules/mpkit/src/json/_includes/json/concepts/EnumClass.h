#pragma once

#include "QEnum.h"

#include <type_traits>

namespace concepts
{

// Check if type T is enum class
template <typename T, bool = (std::is_enum_v<T> && !IsQEnumV<T>)>
struct IsEnumClass : std::false_type
{
};

template <typename T>
struct IsEnumClass<T, true>
  : std::bool_constant<!std::is_convertible_v<T, std::underlying_type_t<T>>>
{
};

template <typename T>
inline constexpr bool IsEnumClassV = IsEnumClass<T>::value;

} // namespace concepts
