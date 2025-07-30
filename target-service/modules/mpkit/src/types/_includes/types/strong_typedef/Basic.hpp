#pragma once

#include <utility>

namespace mpk
{

namespace types
{

/// Simple traits that provides type as is
template <typename T, bool ImplicitCtr = false>
struct TypeIdentity
{
    using ValueType = T;
    static constexpr auto implicitConstructor = ImplicitCtr;
};

/// Strong typedef, holds a single value of type Type
/// Do declare new type one should use
///     class Id : public StrongTypedef<Id, int>
///     {
///         using StrongTypedef::StrongTypedef;
///     };
///
/// Basic version contains only equality operator
/// One should use specific mixins to add desired behaviour
template <typename Tag, typename Traits>
class StrongTypedef
{
    using ValueType = typename Traits::ValueType;

    /// We use this type for SFINAE only
    template <typename ValueType>
    struct Zero : std::integral_constant<int, 0>
    {
    };

public:
    constexpr StrongTypedef() = default;

    template <
        typename OtherValueType,
        int = Zero<std::enable_if_t<
            std::is_constructible_v<
                ValueType,
                std::add_rvalue_reference_t<OtherValueType>> && !Traits::implicitConstructor,
            OtherValueType>>::value>
    constexpr explicit StrongTypedef(OtherValueType value) noexcept(
        std::is_nothrow_constructible_v<ValueType, std::add_rvalue_reference_t<OtherValueType>>) :
      m_value(std::move(value))
    {
    }

    template <
        typename OtherValueType,
        char = Zero<std::enable_if_t<
            std::is_constructible_v<
                ValueType,
                std::add_rvalue_reference_t<OtherValueType>> && Traits::implicitConstructor,
            OtherValueType>>::value>
    constexpr StrongTypedef(OtherValueType value) noexcept( // NOLINT (hicpp-explicit-conversions)
        std::is_nothrow_constructible_v<ValueType, std::add_rvalue_reference_t<OtherValueType>>) :
      m_value(std::move(value))
    {
    }

    constexpr StrongTypedef(const StrongTypedef& other) = default;
    constexpr StrongTypedef(StrongTypedef&& other) = default;
    constexpr StrongTypedef& operator=(const StrongTypedef& other) = default;
    constexpr StrongTypedef& operator=(StrongTypedef&& other) = default;

    constexpr explicit operator ValueType&() noexcept
    {
        return m_value;
    }

    constexpr explicit operator const ValueType&() const noexcept
    {
        return m_value;
    }

    constexpr friend bool operator==(const StrongTypedef& lhs, const StrongTypedef& rhs) noexcept
    {
        return lhs.m_value == rhs.m_value;
    }

    constexpr friend bool operator!=(const StrongTypedef& lhs, const StrongTypedef& rhs) noexcept
    {
        return !(lhs == rhs);
    }

protected:
    constexpr ValueType& value() noexcept
    {
        return m_value;
    }

    constexpr const ValueType& value() const noexcept
    {
        return m_value;
    }

private:
    ValueType m_value{};
};

template <typename Tag, typename Traits>
typename Traits::ValueType underlyingTypeImpl(StrongTypedef<Tag, Traits>);

/// Discover underlying type of strong typedef
template <typename T>
using UnderlyingType = decltype(underlyingTypeImpl(std::declval<T>()));

/// Casts strong type to underlying weak
// NOLINTBEGIN (readability-identifier-naming)
template <typename T>
constexpr const UnderlyingType<T>& weak_cast(const T& v)
{
    return static_cast<const UnderlyingType<T>&>(v);
}

template <typename T>
constexpr UnderlyingType<T>& weak_cast(T& v)
{
    return static_cast<UnderlyingType<T>&>(v);
}
// NOLINTEND (readability-identifier-naming)

} // namespace types

} // namespace mpk
