#pragma once

#include <boost/iterator/iterator_facade.hpp>

#include <memory>

namespace mpkit
{

template <typename T>
class RandomAccessIterator
  : public boost::iterator_facade<RandomAccessIterator<T>, T, boost::random_access_traversal_tag>
{
public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;

private:
    class Inner
    {
    public:
        virtual std::shared_ptr<Inner> clone() const = 0;
        virtual T& dereference() const = 0;
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual void advance(difference_type size) = 0;
        virtual difference_type distanceTo(const Inner& other) const = 0;
        virtual bool equal(const Inner& other) const = 0;
        virtual ~Inner() = default;
    };

public:
    using Private = Inner;

public:
    RandomAccessIterator() = default;

    explicit RandomAccessIterator(const std::shared_ptr<Private>& impl) : m_impl(impl)
    {
    }

    RandomAccessIterator(const RandomAccessIterator& other) :
      m_impl(other.m_impl ? other.m_impl->clone() : std::shared_ptr<Private>())
    {
    }

    RandomAccessIterator& operator=(const RandomAccessIterator& other)
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

    void decrement()
    {
        m_impl->decrement();
    }

    void advance(difference_type size)
    {
        m_impl->advance(size);
    }

    // NOLINTNEXTLINE (readability-identifier-naming)
    difference_type distance_to(const RandomAccessIterator& other) const
    {
        return m_impl->distanceTo(*other.m_impl);
    }

    bool equal(const RandomAccessIterator& other) const
    {
        return m_impl->equal(*other.m_impl);
    }

private:
    std::shared_ptr<Inner> m_impl;
};

} // namespace mpkit
