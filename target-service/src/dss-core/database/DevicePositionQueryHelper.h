#pragma once

#include "dss-core/DevicePosition.h"
#include "dss-core/database/QueryCursorParser.h"
#include "dss-core/database/QueryHelper.h"

#include <optional>

namespace mpk::dss::core
{

template <>
DevicePositionExtended fromQueryCursor<DevicePositionExtended>(
    QueryCursor* cursor);

class DbService;

template <>
class QueryHelper<DevicePositionExtended>
{

public:
    explicit QueryHelper(std::shared_ptr<DbService> dbService);

    std::vector<DevicePositionExtended> fetchData();
    std::unique_ptr<QueryCursor> data();

    void append(const DevicePositionExtended& value);
    void replace(const QString& key, const DevicePositionExtended& value);
    void remove(const QString& key);
    std::optional<DevicePositionExtended> find(const QString& key);
    void clear();

private:
    std::shared_ptr<DbService> m_dbService;
};

} // namespace mpk::dss::core
