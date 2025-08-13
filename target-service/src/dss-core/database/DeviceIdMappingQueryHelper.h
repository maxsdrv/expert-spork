#pragma once

#include "dss-core/DeviceIdMapping.h"
#include "dss-core/database/QueryCursorParser.h"
#include "dss-core/database/QueryHelper.h"

namespace mpk::dss::core
{

template <>
DeviceIdMapping fromQueryCursor<DeviceIdMapping>(QueryCursor* cursor);

class DbService;

template <>
class QueryHelper<DeviceIdMapping>
{

public:
    explicit QueryHelper(std::shared_ptr<DbService> dbService);

    std::vector<DeviceIdMapping> fetchData();
    std::unique_ptr<QueryCursor> data();

    void append(const DeviceIdMapping& value);
    void replace(const QString& key, const DeviceIdMapping& value);
    void remove(const QString& key);
    std::optional<DeviceIdMapping> find(const QString& key);
    void clear();

private:
    std::shared_ptr<DbService> m_dbService;
};

} // namespace mpk::dss::core
