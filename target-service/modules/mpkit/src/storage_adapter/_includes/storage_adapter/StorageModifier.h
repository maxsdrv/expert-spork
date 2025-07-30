#pragma once

#include "storage_adapter/ProviderSignals.h"

#include "gsl/pointers"

namespace mpk
{

namespace storage_adapter
{

template <typename Storage, bool exceptionSafe = true>
class StorageModifier
{
public:
    using Iterator = typename Storage::Iterator;

    explicit StorageModifier(gsl::not_null<Storage*> storage) :
      m_storage{storage}, m_pin{m_storage->pin()}
    {
    }

    ~StorageModifier() noexcept
    {
        // Emitting signals in dtor from exception handlers is naughty.
        if (!exceptionSafe || !std::current_exception())
        {
            emit m_pin->editingFinished();
        }
    }

    template <typename ValueType>
    void append(ValueType value)
    {
        emit m_pin->aboutToAppend();
        m_storage->append(std::forward<ValueType>(value));
        emit m_pin->appendCompleted(static_cast<int>(m_storage->count() - 1));
    }

    template <typename Iter>
    void append(Iter first, Iter last)
    {
        for (auto iter = first; iter != last; ++iter)
        {
            append(*iter);
        }
    }

    template <typename ValueType>
    void replace(const Iterator& iter, ValueType value)
    {
        if (iter != m_storage->end())
        {
            auto index = std::distance(m_storage->begin(), iter);
            m_storage->replace(index, std::forward<ValueType>(value));
            emit m_pin->changed(index);
        }
        else
        {
            append(std::forward<ValueType>(value));
        }
    }

    Iterator remove(const Iterator& iter)
    {
        if (iter != m_storage->end())
        {
            auto index = std::distance(m_storage->begin(), iter);
            emit m_pin->aboutToRemove(index);
            m_storage->remove(index);
            emit m_pin->removeCompleted();
            return std::next(m_storage->begin(), index);
        }
        return iter;
    }

    template <typename Predicate>
    void removeIf(const Predicate& predicate)
    {
        for (auto i = 0; i < m_storage->count();)
        {
            if (predicate(m_storage->at(i)))
            {
                emit m_pin->aboutToRemove(i);
                m_storage->remove(i);
                emit m_pin->removeCompleted();
            }
            else
            {
                ++i;
            }
        }
    }

    void clear() noexcept
    {
        emit m_pin->aboutToReset();
        m_storage->clear();
        emit m_pin->resetCompleted();
    }

private:
    gsl::not_null<Storage*> m_storage;
    gsl::not_null<ProviderSignals*> m_pin;
};

} // namespace storage_adapter

} // namespace mpk
