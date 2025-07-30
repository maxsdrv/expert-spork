#pragma once

#include "Basic.hpp"

#include <functional>

namespace mpk::types
{

// Use this traits to automatically add std::hash function for strong type if
// underlying type either has std::hash<T> or qHash(T).
template <typename T>
class Hashable
{
};

// clang-format off
template <typename T>
concept HasStdHash = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept StdHashable = std::is_base_of_v<Hashable<T>, T> && HasStdHash<UnderlyingType<T>>;

// clang-format on

} // namespace mpk::types

template <mpk::types::StdHashable T>
struct std::hash<T>
{
    std::size_t operator()(const T& value) const noexcept
    {
        return std::hash<mpk::types::UnderlyingType<T>>{}(mpk::types::weak_cast(value));
    }
};
