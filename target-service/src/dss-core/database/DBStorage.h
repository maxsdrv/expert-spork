#pragma once

#include "dss-core/Provider.h"
#include "dss-core/StorageModifier.h"

#include "dss-core/database/DBCursorForwardIterator.h"
#include "dss-core/database/QueryHelper.h"

#include "gsl/assert"

#include <boost/optional.hpp>

#include <algorithm>
#include <vector>

namespace mpk::dss::core
{

class DbService;

template <typename T>
class DBStorage;

template <typename T>
struct DBStorageTraits
{
    using ValueType = T;
    using Iterator = mpkit::ForwardIterator<ValueType const>;
    using Modifier = StorageModifier<DBStorage<ValueType>>;
};

template <typename T>
class DBStorage : public Provider<DBStorage<T>, DBStorageTraits<T>>
{
public:
    using Base = Provider<DBStorage<T>, DBStorageTraits<T>>;
    using Traits = DBStorageTraits<T>;
    using ValueType = typename Traits::ValueType;
    using Iterator = typename Traits::Iterator;
    using Modifier = typename Traits::Modifier;
    using Data = std::vector<ValueType>;

    friend class StorageModifier<DBStorage<ValueType>>;

private:
    using PrivateIterator = DBCursorForwardIterator<ValueType const>;

public:
    explicit DBStorage(std::shared_ptr<DbService> dbService) :
      Base(),
      m_queryHelper(std::make_unique<QueryHelper<T>>(std::move(dbService))),
      m_pin(std::make_unique<ProviderSignals>())
    {
    }
    virtual ~DBStorage() = default;

    DBStorage(DBStorage&& other) noexcept :
      m_queryHelper(std::move(other.queryHelper)),
      m_data(std::move(other.m_data)),
      m_pin(std::move(other.m_pin))
    {
    }
    DBStorage& operator=(DBStorage&& other) noexcept
    {
        m_queryHelper = std::move(other.queryHelper);
        m_data = std::move(other.m_data);
        m_pin = std::move(other.m_pin);
        return *this;
    }

    using Base::find;

    [[nodiscard]] std::optional<ValueType> find(
        const QString& key) const override
    {
        return m_queryHelper->find(key);
    }
    [[nodiscard]] gsl::not_null<ProviderSignals*> pin() const noexcept
    {
        return m_pin.get();
    }
    [[nodiscard]] std::shared_ptr<Modifier> modifier() noexcept
    {
        return std::make_shared<Modifier>(this);
    }
    [[nodiscard]] std::vector<T> fetchData() noexcept
    {
        return m_queryHelper->fetchData();
    }
    void clear()
    {
        m_queryHelper->clear();
    }

private:
    void append(const T& value)
    {
        m_queryHelper->append(value);
    }
    void replace(const QString& key, [[maybe_unused]] const T& value)
    {
        m_queryHelper->replace(key, value);
    }
    void remove(const Iterator& iter)
    {
        m_queryHelper->remove(iter->key);
    }

private:
    mutable std::unique_ptr<QueryHelper<T>> m_queryHelper;
    mutable Data m_data;
    std::unique_ptr<ProviderSignals> m_pin;
};

} // namespace mpk::dss::core
