#pragma once

#include "storage_adapter/Provider.h"
#include "storage_adapter/StorageModifier.h"
#include "storage_adapter/details/ProxyIterator.h"

namespace mpk
{

namespace storage_adapter
{

template <typename T>
class ProxyStorage;

template <typename T>
struct ProxyStorageTraits
{
    using ValueType = T;
    using Iterator = mpkit::RandomAccessIterator<const ValueType>;
    using Modifier = StorageModifier<ProxyStorage<ValueType>>;
};

template <typename T>
class ProxyStorage : public Provider<ProxyStorage<T>, ProxyStorageTraits<T>>
{
public:
    using Base = Provider<ProxyStorage<T>, ProxyStorageTraits<T>>;
    using Traits = ProxyStorageTraits<T>;
    using ValueType = typename Traits::ValueType;
    using Iterator = typename Traits::Iterator;
    using Modifier = typename Traits::Modifier;

    friend Modifier;

private:
    using PrivateIterator = ProxyIterator<const ValueType>;

public:
    explicit ProxyStorage(ProxyObject<ValueType> proxy) : Base(), m_proxy{std::move(proxy)}
    {
    }

    ProxyStorage(ProxyStorage&& other) noexcept = default;
    ProxyStorage& operator=(ProxyStorage&& other) noexcept = default;

    [[nodiscard]] int count() const
    {
        return m_proxy.size();
    }

    [[nodiscard]] const ValueType& at(int index) const
    {
        return m_proxy.value(index);
    }

    [[nodiscard]] Iterator begin() const
    {
        return Iterator{std::make_shared<PrivateIterator>(&m_proxy, 0)};
    }

    [[nodiscard]] Iterator end() const
    {
        return Iterator{std::make_shared<PrivateIterator>(&m_proxy, m_proxy.size())};
    }

    template <typename Predicate>
    [[nodiscard]] Iterator find(const Predicate& predicate) const
    {
        auto i = 0;
        for (; i < m_proxy.size(); i++)
        {
            if (predicate(m_proxy.value(i)))
            {
                break;
            }
        }
        return Iterator{std::make_shared<PrivateIterator>(&m_proxy, i)};
    }

private:
    void append(const ValueType& value)
    {
        m_proxy.append(value);
        m_proxy.update();
    }

    void replace(int index, const ValueType& value)
    {
        m_proxy.replace(index, value);
        m_proxy.update();
    }

    void remove(int index)
    {
        m_proxy.remove(index);
        m_proxy.update();
    }

    void clear()
    {
        m_proxy.clear();
        m_proxy.update();
    }

private:
    mutable ProxyObject<ValueType> m_proxy;
};

} // namespace storage_adapter

} // namespace mpk
