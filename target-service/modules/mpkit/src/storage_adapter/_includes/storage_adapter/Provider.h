#pragma once

#include "storage_adapter/ProviderSignals.h"

#include "gsl/pointers"

#include <memory>

namespace mpk
{

namespace storage_adapter
{

template <typename Storage, typename Traits>
class Provider
{
public:
    using Iterator = typename Traits::Iterator;
    using ValueType = typename Traits::ValueType;
    using Modifier = typename Traits::Modifier;

public:
    Provider() : m_pin{std::make_unique<ProviderSignals>()}
    {
    }

    Provider(const Provider& other) = delete;
    Provider& operator=(const Provider& other) = delete;
    Provider(Provider&& other) noexcept = default;
    Provider& operator=(Provider&& other) noexcept = default;

    [[nodiscard]] int count() const noexcept
    {
        return static_cast<const Storage&>(*this).count();
    }

    [[nodiscard]] const ValueType& at(int index) const
    {
        return static_cast<const Storage&>(*this).at(index);
    }

    [[nodiscard]] Iterator begin() const noexcept
    {
        return static_cast<const Storage&>(*this).begin();
    }

    [[nodiscard]] Iterator end() const noexcept
    {
        return static_cast<const Storage&>(*this).end();
    }

    template <typename Predicate>
    [[nodiscard]] Iterator find(const Predicate& predicate) const
    {
        return static_cast<const Storage&>(*this).find(predicate);
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return count() == 0;
    }

    [[nodiscard]] int indexOf(const ValueType& value) const noexcept
    {
        auto it = find([value](const auto& _value) { return _value == value; });
        return it != end() ? std::distance(begin(), it) : -1;
    }

    [[nodiscard]] bool contains(const ValueType& value) const noexcept
    {
        auto it = find([value](const auto& _value) { return _value == value; });
        return it != end();
    }

    [[nodiscard]] gsl::not_null<ProviderSignals*> pin() const noexcept
    {
        return m_pin.get();
    }

    [[nodiscard]] std::unique_ptr<Modifier> modifier() noexcept
    {
        return std::make_unique<Modifier>(static_cast<Storage*>(this));
    }

private:
    std::unique_ptr<ProviderSignals> m_pin;
};

} // namespace storage_adapter

} // namespace mpk
