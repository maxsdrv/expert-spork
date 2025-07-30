#pragma once

#include "boost/iterators/RandomAccessIterator.h"

namespace mpkit::details
{

template <typename IteratorType, typename T>
class ValueTypeRandomAccessIterator : public RandomAccessIterator<T>::Private
{
public:
    using PrivateBaseRandomAccessIterator = typename RandomAccessIterator<T>::Private;
    using DiffType = typename RandomAccessIterator<T>::difference_type;

    explicit ValueTypeRandomAccessIterator(IteratorType internal) : m_internal(internal)
    {
    }

    T& dereference() const override
    {
        return *m_internal;
    }

    void increment() override
    {
        ++m_internal;
    }

    void decrement() override
    {
        --m_internal;
    }

    void advance(DiffType size) override
    {
        std::advance(m_internal, size);
    }

    DiffType distanceTo(const PrivateBaseRandomAccessIterator& other) const override
    {
        return std::distance(
            m_internal, static_cast<const ValueTypeRandomAccessIterator&>(other).m_internal);
    }

    bool equal(const PrivateBaseRandomAccessIterator& other) const override
    {
        return m_internal == static_cast<const ValueTypeRandomAccessIterator&>(other).m_internal;
    }

    std::shared_ptr<PrivateBaseRandomAccessIterator> clone() const override
    {
        return std::make_shared<ValueTypeRandomAccessIterator>(m_internal);
    }

private:
    IteratorType m_internal;
};

} // namespace mpkit::details
