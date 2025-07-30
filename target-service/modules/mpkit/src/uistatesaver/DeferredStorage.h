/** @file
 * @brief   KeyValueStorage implementation that caches
 *          value changes and syncs them once in a period of time
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/KeyValueStorage.h"

#include "qt/DeferredCall.h"

#include "gsl/assert"

#include <map>
#include <memory>

namespace uistatesaver
{

/**
 * This class caches value changes and syncs them once in a period of time
 */
template <class Key, class Value>
class DeferredStorage : public KeyValueStorage<Key, Value>
{
    struct Item
    {
        Value value;
        bool synced;
    };

    using Storage = std::map<Key, Item>;

public:
    DeferredStorage(int syncTimeout, const std::shared_ptr<KeyValueStorage<Key, Value>>& backend) :
      m_backend(backend),
      m_sync(std::make_unique<qt::DeferredCall>(
          std::bind(&DeferredStorage<Key, Value>::sync, this), syncTimeout))
    {
        Expects(m_backend != nullptr);
    }

    ~DeferredStorage()
    {
        sync();
    }

    /**
     * KeyValueStorage methods implementation
     */
    void set(const Key& id, const Value& value) override
    {
        auto it = m_storage.find(id);
        if (it != m_storage.end())
        {
            it->second.value = value;
            it->second.synced = false;
        }
        else
        {
            Item item;
            item.value = value;
            item.synced = false;
            m_storage.insert(std::make_pair(id, item));
        }

        m_sync->run();
    }

    /**
     * Return state for specified id
     */
    Value get(const Key& id) override
    {
        auto it = m_storage.find(id);
        if (it == m_storage.end())
        {
            Item item;
            item.value = m_backend->get(id);
            item.synced = true;
            it = m_storage.insert(std::make_pair(id, item)).first;
        }

        return it->second.value;
    }

private:
    void sync()
    {
        for (auto& item: m_storage)
        {
            if (!item.second.synced)
            {
                m_backend->set(item.first, item.second.value);
                item.second.synced = true;
            }
        }
    }

private:
    Storage m_storage;
    std::shared_ptr<KeyValueStorage<Key, Value>> m_backend;
    std::unique_ptr<qt::DeferredCall> m_sync;
};

} // namespace uistatesaver
