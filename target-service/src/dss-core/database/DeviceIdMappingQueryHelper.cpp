#include "DeviceIdMappingQueryHelper.h"

#include "dss-core/database/DbService.h"

namespace mpk::dss::core
{

namespace
{

constexpr auto selectAllQuery =
    "SELECT "
    "fingerprint, id, class "
    "FROM device";

} // namespace

template <>
DeviceIdMapping fromQueryCursor<DeviceIdMapping>(QueryCursor* cursor)
{
    auto fingerprint = cursor->nextValue<QString>();
    auto id = cursor->nextValue<QString>();
    auto type = DeviceClass::fromString(cursor->nextValue<QString>());
    auto key = DeviceIdMappingKey{fingerprint, type};
    return {DeviceIdMappingKey::toString(key), DeviceId(id)};
}

QueryHelper<DeviceIdMapping>::QueryHelper(std::shared_ptr<DbService> dbService) :
  m_dbService(std::move(dbService))
{
}

std::vector<DeviceIdMapping> QueryHelper<DeviceIdMapping>::fetchData()
{
    std::vector<DeviceIdMapping> data;

    auto query = m_dbService->exec(selectAllQuery);
    while (query->next())
    {
        auto row = query->rowValue<DeviceIdMapping>();
        data.push_back(row);
    }
    return data;
}

std::unique_ptr<QueryCursor> QueryHelper<DeviceIdMapping>::data()
{
    return m_dbService->exec(selectAllQuery);
}

void QueryHelper<DeviceIdMapping>::append(const DeviceIdMapping& value)
{
    constexpr auto queryStr =
        "INSERT INTO device "
        "(fingerprint, id, class) "
        "VALUES "
        "(:fingerprint, :id, :class)";
    auto query = m_dbService->prepare(queryStr);

    auto key = DeviceIdMappingKey::fromString(value.key);
    query->bindValue(":fingerprint", key.fingerprint);
    query->bindValue(":id", static_cast<QString>(value.id));
    query->bindValue(":class", DeviceClass::toString(key.type));
    m_dbService->exec(*query);
}

void QueryHelper<DeviceIdMapping>::replace(
    const QString& key, const DeviceIdMapping& value)
{
    constexpr auto queryStr =
        "UPDATE device "
        "SET id=:id, "
        "WHERE fingerprint=:fingerprint "
        "AND class=:class";
    auto query = m_dbService->prepare(queryStr);

    auto mappingKey = DeviceIdMappingKey::fromString(key);
    query->bindValue(":fingerprint", mappingKey.fingerprint);
    query->bindValue(":id", static_cast<QString>(value.id));
    query->bindValue(":class", DeviceClass::toString(mappingKey.type));
    m_dbService->exec(*query);
}

void QueryHelper<DeviceIdMapping>::remove(const QString& key)
{
    constexpr auto queryStr =
        "DELETE FROM device "
        "WHERE fingerprint=:fingerprint "
        "AND class=:class";
    auto query = m_dbService->prepare(queryStr);

    auto mappingKey = DeviceIdMappingKey::fromString(key);
    query->bindValue(":fingerprint", mappingKey.fingerprint);
    query->bindValue(":class", DeviceClass::toString(mappingKey.type));
    m_dbService->exec(*query);
}

void QueryHelper<DeviceIdMapping>::clear()
{
    constexpr auto queryStr = "DELETE FROM device";
    m_dbService->exec(queryStr);
}

std::optional<DeviceIdMapping> QueryHelper<DeviceIdMapping>::find(
    const QString& key)
{
    constexpr auto queryStr =
        "SELECT fingerprint, id, class "
        "FROM device "
        "WHERE fingerprint=:fingerprint "
        "AND class=:class";
    auto query = m_dbService->prepare(queryStr);

    auto mappingKey = DeviceIdMappingKey::fromString(key);
    query->bindValue(":fingerprint", mappingKey.fingerprint);
    query->bindValue(":class", DeviceClass::toString(mappingKey.type));
    m_dbService->exec(*query);

    if (query->next())
    {
        return query->rowValue<DeviceIdMapping>();
    }
    return std::nullopt;
}

} // namespace mpk::dss::core
