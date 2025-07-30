#pragma once

#include <boost/iterator/iterator_facade.hpp>

#include <memory>

namespace mpkit
{

template <typename T>
class ForwardIterator
  : public boost::iterator_facade<ForwardIterator<T>, T, boost::forward_traversal_tag>
{

private:
    /**
     * Interface for concrete iterator
     */
    class _inner // NOLINT (readability-identifier-naming)
    {
    public:
        virtual std::shared_ptr<_inner> clone() const = 0;
        virtual T& dereference() const = 0;
        virtual void increment() = 0;
        virtual bool equal(const _inner& other) const = 0;
        virtual ~_inner() = default;
    };

public:
    using value_type = T;
    using Private = _inner;

public:
    /**
     * Empty iterator. Dereference of it cause UB
     */
    ForwardIterator() = default;

    explicit ForwardIterator(const std::shared_ptr<Private>& impl) : m_impl(impl)
    {
    }

    ForwardIterator(const ForwardIterator& other) :
      m_impl(other.m_impl ? other.m_impl->clone() : std::shared_ptr<Private>())
    {
    }

    ForwardIterator& operator=(const ForwardIterator& other)
    {
        if (this != &other)
        {
            m_impl = other.m_impl ? other.m_impl->clone() : std::shared_ptr<Private>();
        }
        return *this;
    }

private:
    friend class boost::iterator_core_access;

    value_type& dereference() const
    {
        return m_impl->dereference();
    }

    void increment()
    {
        m_impl->increment();
    }

    bool equal(const ForwardIterator& other) const
    {
        return m_impl->equal(*other.m_impl);
    }

private:
    std::shared_ptr<Private> m_impl;
};

} // namespace mpkit
