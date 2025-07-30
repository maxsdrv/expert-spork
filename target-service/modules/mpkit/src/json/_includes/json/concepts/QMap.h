#pragma once

#include <QMap>

namespace concepts
{

template <typename... Args>
struct IsQMap : std::false_type
{
};

template <typename... Args>
struct IsQMap<QMap<Args...>> : std::true_type
{
};

template <typename T>
inline constexpr bool IsQMapV = IsQMap<T>::value;

} // namespace concepts
