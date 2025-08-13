#pragma once

#include "dss-core/database/QueryCursor.h"

#include "boost/iterators/ForwardIterator.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

template <typename T>
class DBCursorForwardIterator : public mpkit::ForwardIterator<T>::Private
{
public:
    using PrivateBaseForwardIterator =
        typename mpkit::ForwardIterator<T>::Private;

    explicit DBCursorForwardIterator(
        gsl::not_null<QueryCursor*> internal, int index) :
      m_index(index), m_cursor(internal)
    {
    }

    T& dereference() const override
    {
        if (m_value)
        {
            return *m_value;
        }
        m_cursor->seek(m_index);
        m_value = m_cursor->rowValue<T>();
        return *m_value;
    }

    void increment() override
    {
        ++m_index;
        m_value = boost::none;
    }

    bool equal(const PrivateBaseForwardIterator& other) const override
    {
        return m_index
               == static_cast<const DBCursorForwardIterator&>(other).m_index;
    }

    std::shared_ptr<PrivateBaseForwardIterator> clone() const override
    {
        return std::make_shared<DBCursorForwardIterator>(m_cursor, m_index);
    }

private:
    int m_index = 0;
    mutable gsl::not_null<QueryCursor*> m_cursor;
    mutable boost::optional<std::remove_const_t<T>> m_value = boost::none;
};

} // namespace mpk::dss::core
