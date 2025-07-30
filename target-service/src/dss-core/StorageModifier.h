#pragma once

#include "ProviderSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

template <typename Storage>
class StorageModifier
{
public:
    using Iterator = typename Storage::Iterator;
    using ValueType = typename Storage::ValueType;

    explicit StorageModifier(gsl::not_null<Storage*> storage) :
      m_storage(storage), m_pin(m_storage->pin())
    {
    }

    ~StorageModifier() noexcept
    {
        emit m_pin->modified();
    }

    void append(const ValueType& value)
    {
        emit m_pin->aboutToAppend();
        m_storage->append(value);
        emit m_pin->appendCompleted(value.key);
    }

    template <typename Iter>
    void append(Iter first, Iter last)
    {
        for (auto iter = first; iter != last; ++iter)
        {
            append(*iter);
        }
    }

    void replace(const QString& key, const ValueType& value)
    {
        if (m_storage->find(key).has_value())
        {
            m_storage->replace(key, value);
            emit m_pin->changed(key);
        }
        else
        {
            append(value);
        }
    }

    Iterator remove(const Iterator& iter)
    {
        auto it = iter;
        if (it != m_storage->end())
        {
            emit m_pin->aboutToRemove(it->key);
            it = m_storage->remove(it);
            emit m_pin->removeCompleted();
        }
        return it;
    }

    template <typename Predicate>
    void removeAll(const Predicate& predicate)
    {
        auto findNext = [this, predicate](const auto& itBegin)
        { return std::find_if(itBegin, m_storage->end(), predicate); };

        for (auto it = findNext(m_storage->begin()); it != m_storage->end();
             it = findNext(it))
        {
            emit m_pin->aboutToRemove(it->key);
            it = m_storage->remove(it);
            emit m_pin->removeCompleted();
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

} // namespace mpk::dss::core
