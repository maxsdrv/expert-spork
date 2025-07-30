#pragma once

#include <QString>

#include "gsl/pointers"

#include <memory>
#include <optional>

namespace mpk::dss::core
{

class ProviderSignals;

template <typename Storage, typename Traits>
class Provider
{
public:
    using Iterator = typename Traits::Iterator;
    using ValueType = typename Traits::ValueType;
    using Modifier = typename Traits::Modifier;

public:
    Provider() = default;
    Provider(const Provider& other) = delete;
    Provider& operator=(const Provider& other) = delete;

    [[nodiscard]] int count() const noexcept
    {
        return static_cast<const Storage&>(*this).count();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return static_cast<const Storage&>(*this).empty();
    }

    [[nodiscard]] virtual std::optional<ValueType> find(
        [[maybe_unused]] const QString& key) const
    {
        return {};
    }

    [[nodiscard]] bool contains(const QString& key) const
    {
        return find(key).has_value();
    }

    [[nodiscard]] gsl::not_null<ProviderSignals*> pin() const noexcept
    {
        return static_cast<const Storage&>(*this).pin();
    }

    [[nodiscard]] std::shared_ptr<Modifier> modifier() noexcept
    {
        return static_cast<Storage&>(*this).modifier();
    }
};

} // namespace mpk::dss::core
