#pragma once

#include <QMetaEnum>

namespace concepts
{

template <typename T>
inline constexpr bool IsQEnumV = QtPrivate::IsQEnumHelper<T>::Value;

} // namespace concepts
