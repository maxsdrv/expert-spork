#pragma once

#include "storage_adapter/Provider.h"
#include "storage_adapter/StorageModifier.h"

#include "boost/iterators/details/ValueTypeRandomAccessIterator.h"

#include "gsl/assert"

#include <algorithm>
#include <vector>

namespace mpk
{

namespace storage_adapter
{

template <typename T>
class MemoryStorage;

template <typename T>
struct MemoryStorageTraits
{
    using ValueType = T;
    using Iterator = mpkit::RandomAccessIterator<const ValueType>;
    using Modifier = StorageModifier<MemoryStorage<ValueType>>;
};

template <typename T>
class MemoryStorage : public Provider<MemoryStorage<T>, MemoryStorageTraits<T>>
{
public:
    using Base = Provider<MemoryStorage<T>, MemoryStorageTraits<T>>;
    using Traits = MemoryStorageTraits<T>;
    using ValueType = typename Traits::ValueType;
    using Iterator = typename Traits::Iterator;
    using Modifier = typename Traits::Modifier;
    using Data = std::vector<ValueType>;

    friend Modifier;

private:
    using PrivateIterator = mpkit::details::
        ValueTypeRandomAccessIterator<typename Data::const_iterator, const ValueType>;

public:
    explicit MemoryStorage() : Base()
    {
    }

    MemoryStorage(MemoryStorage&& other) noexcept = default;
    MemoryStorage& operator=(MemoryStorage&& other) noexcept = default;

    [[nodiscard]] int count() const noexcept
    {
        return static_cast<int>(m_data.size());
    }

    [[nodiscard]] const ValueType& at(int index) const
    {
        Expects(index >= 0);
        Expects(index < static_cast<int>(m_data.size()));
        return m_data.at(index);
    }

    [[nodiscard]] Iterator begin() const noexcept
    {
        return Iterator{std::make_shared<PrivateIterator>(m_data.begin())};
    }

    [[nodiscard]] Iterator end() const noexcept
    {
        return Iterator{std::make_shared<PrivateIterator>(m_data.end())};
    }

    template <typename Predicate>
    [[nodiscard]] Iterator find(const Predicate& predicate) const
    {
        auto it = std::find_if(m_data.begin(), m_data.end(), predicate);
        return Iterator{std::make_shared<PrivateIterator>(it)};
    }

private:
    template <typename VT>
    void append(VT value)
    {
        m_data.emplace_back(std::forward<VT>(value));
    }

    template <typename VT>
    void replace(int index, VT value)
    {
        Expects(index >= 0);
        Expects(index < static_cast<int>(m_data.size()));
        std::swap(m_data.at(index), value);
    }

    void remove(int index)
    {
        Expects(index >= 0);
        Expects(index < static_cast<int>(m_data.size()));
        m_data.erase(std::next(m_data.begin(), index));
    }

    void clear() noexcept
    {
        m_data.clear();
    }

private:
    Data m_data;
};

} // namespace storage_adapter

} // namespace mpk
