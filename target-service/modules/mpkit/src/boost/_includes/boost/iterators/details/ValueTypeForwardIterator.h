#pragma once

#include "boost/iterators/ForwardIterator.h"

namespace mpkit::details
{

template <typename IteratorType, typename T>
class ValueTypeForwardIterator : public ForwardIterator<T>::Private
{
public:
    using PrivateBaseForwardIterator = typename ForwardIterator<T>::Private;

    explicit ValueTypeForwardIterator(IteratorType internal) : m_internal(internal)
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

    bool equal(const PrivateBaseForwardIterator& other) const override
    {
        return m_internal == static_cast<const ValueTypeForwardIterator&>(other).m_internal;
    }

    std::shared_ptr<PrivateBaseForwardIterator> clone() const override
    {
        return std::make_shared<ValueTypeForwardIterator>(m_internal);
    }

private:
    IteratorType m_internal;
};

} // namespace mpkit::details
