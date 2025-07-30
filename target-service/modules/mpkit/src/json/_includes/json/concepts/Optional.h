#pragma once

#include <optional>

namespace concepts
{

template <typename T>
struct IsOptional : std::false_type
{
};

template <typename T>
struct IsOptional<std::optional<T>> : std::true_type
{
};

template <typename T>
inline constexpr bool IsOptionalV = IsOptional<T>::value;

} // namespace concepts
