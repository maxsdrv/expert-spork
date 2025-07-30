#pragma once

#include "Basic.hpp"

namespace mpk
{

namespace types
{

/// Adds order operators to strong typedef
template <typename StrongType>
class Ordered
{
public:
    constexpr friend bool operator<(const StrongType& lhs, const StrongType& rhs) noexcept
    {
        return weak_cast(lhs) < weak_cast(rhs);
    }

    constexpr friend bool operator>(const StrongType& lhs, const StrongType& rhs) noexcept
    {
        return weak_cast(lhs) > weak_cast(rhs);
    }

    constexpr friend bool operator<=(const StrongType& lhs, const StrongType& rhs) noexcept
    {
        return !(lhs > rhs);
    }

    constexpr friend bool operator>=(const StrongType& lhs, const StrongType& rhs) noexcept
    {
        return !(lhs < rhs);
    }
};

/// Adds plus and minus operations
template <typename StrongType, typename DifferenceType = StrongType>
class Additive
{

public:
    constexpr friend auto operator+(StrongType left, const DifferenceType& right) noexcept
    {
        left += right;
        return left;
    }
    constexpr auto& operator+=(const DifferenceType& other) noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        weak_cast(self) += weak_cast(other);
        return self;
    }

    constexpr friend StrongType operator-(StrongType left, const DifferenceType& right) noexcept
    {
        left -= right;
        return left;
    }
    constexpr auto& operator-=(const DifferenceType& other) noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        weak_cast(self) -= weak_cast(other);
        return self;
    }

    template <typename S>
    constexpr friend std::enable_if_t<std::is_same_v<S, StrongType>, DifferenceType> operator-(
        const S& left, const S& right) noexcept
    {
        return DifferenceType(weak_cast(left) - weak_cast(right));
    }
};

/// Adds multiply and divide by scalar
template <typename StrongType>
class Scalable
{
public:
    template <typename T>
    constexpr friend auto operator*(StrongType unit, T value) noexcept
    {
        unit *= value;
        return unit;
    }

    template <typename T>
    constexpr auto& operator*=(T value) noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        weak_cast(self) *= value;
        return self;
    }

    template <typename T>
    constexpr friend auto operator/(StrongType unit, T value) noexcept
    {
        unit /= value;
        return unit;
    }

    template <typename T>
    constexpr auto& operator/=(T value) noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        weak_cast(self) /= value;
        return self;
    }

    constexpr friend double operator/(const StrongType& left, const StrongType& right) noexcept
    {
        return static_cast<double>(weak_cast(left)) / weak_cast(right);
    }
};

/// Adds increment and decrement operations
template <typename StrongType>
class Incrementable
{
public:
    constexpr auto& operator++() noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        ++weak_cast(self);
        return self;
    }

    constexpr auto operator++(int) noexcept
    {
        auto result = *static_cast<StrongType*>(this);
        auto& self = *static_cast<StrongType*>(this);
        ++weak_cast(self);
        return result;
    }

    constexpr auto& operator--() noexcept
    {
        auto& self = *static_cast<StrongType*>(this);
        --weak_cast(self);
        return self;
    }

    constexpr auto operator--(int) noexcept
    {
        auto result = *static_cast<StrongType*>(this);
        auto& self = *static_cast<StrongType*>(this);
        --weak_cast(self);
        return result;
    }
};

} // namespace types

} // namespace mpk
