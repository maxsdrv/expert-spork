#pragma once

#include "storage_adapter/details/ProxyObject.h"

#include "boost/iterators/RandomAccessIterator.h"

#include "gsl/pointers"

#include <optional>

namespace mpk
{

namespace storage_adapter
{

template <typename T>
class ProxyIterator : public mpkit::RandomAccessIterator<T>::Private
{
public:
    using PrivateBase = typename mpkit::RandomAccessIterator<T>::Private;
    using DiffType = typename mpkit::RandomAccessIterator<T>::difference_type;
    using ValueType = typename std::remove_cv_t<T>;

    explicit ProxyIterator(gsl::not_null<ProxyObject<ValueType>*> proxy, int index = 0) :
      m_proxy{proxy}, m_index{index}
    {
    }

    T& dereference() const override
    {
        if (!m_value)
        {
            m_value = m_proxy->value(m_index);
        }
        return *m_value;
    }

    void increment() override
    {
        ++m_index;
        m_value = std::nullopt;
    }

    void decrement() override
    {
        --m_index;
        m_value = std::nullopt;
    }

    void advance(DiffType size) override
    {
        m_index += size;
        m_value = std::nullopt;
    }

    DiffType distanceTo(const PrivateBase& other) const override
    {
        return m_index - static_cast<const ProxyIterator&>(other).m_index;
    }

    bool equal(const PrivateBase& other) const override
    {
        return m_index == static_cast<const ProxyIterator&>(other).m_index;
    }

    std::shared_ptr<PrivateBase> clone() const override
    {
        return std::make_shared<ProxyIterator>(m_proxy, m_index);
    }

private:
    gsl::not_null<ProxyObject<ValueType>*> m_proxy;
    int m_index = 0;
    mutable std::optional<ValueType> m_value = std::nullopt;
};

} // namespace storage_adapter

} // namespace mpk
