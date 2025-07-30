#include "DevicePositionQueryHelper.h"

#include "dss-core/database/DbService.h"

namespace mpk::dss::core
{

namespace
{

constexpr auto selectAllQuery =
    "SELECT "
    "device_id, mode, latitude, longitude, altitude, azimuth "
    "FROM device_position";

} // namespace

template <>
DevicePositionExtended fromQueryCursor<DevicePositionExtended>(
    QueryCursor* cursor)
{
    auto id = cursor->nextValue<QString>();
    auto mode =
        OAService::OAServiceGeo_position_mode(cursor->nextValue<QString>());
    auto latitude = cursor->nextValue<double>();
    auto longitude = cursor->nextValue<double>();
    auto altitude = cursor->nextValue<double>();
    auto azimuth = cursor->nextValue<double>();
    return {
        .key = id,
        .mode = mode.getValue(),
        .position = DevicePosition{
            .azimuth = azimuth,
            .coordinate = QGeoCoordinate(latitude, longitude, altitude)}};
}

QueryHelper<DevicePositionExtended>::QueryHelper(
    std::shared_ptr<DbService> dbService) :
  m_dbService(std::move(dbService))
{
}

std::vector<DevicePositionExtended> QueryHelper<
    DevicePositionExtended>::fetchData()
{
    std::vector<DevicePositionExtended> data;

    auto query = m_dbService->exec(selectAllQuery);
    while (query->next())
    {
        auto row = query->rowValue<DevicePositionExtended>();
        data.push_back(row);
    }
    return data;
}

std::unique_ptr<QueryCursor> QueryHelper<DevicePositionExtended>::data()
{
    return m_dbService->exec(selectAllQuery);
}

void QueryHelper<DevicePositionExtended>::append(
    const DevicePositionExtended& value)
{
    constexpr auto queryStr =
        "INSERT INTO device_position "
        "(device_id, mode, latitude, longitude, altitude, azimuth) "
        "VALUES "
        "(:device_id, :mode, :latitude, :longitude, :altitude, :azimuth)";
    auto query = m_dbService->prepare(queryStr);

    query->bindValue(":device_id", value.key);
    OAService::OAServiceGeo_position_mode mode;
    mode.setValue(value.mode);
    query->bindValue(":mode", mode.asJson());
    query->bindValue(":latitude", value.position.coordinate.latitude());
    query->bindValue(":longitude", value.position.coordinate.longitude());
    query->bindValue(":altitude", value.position.coordinate.altitude());
    query->bindValue(":azimuth", value.position.azimuth);
    m_dbService->exec(*query);
}

void QueryHelper<DevicePositionExtended>::replace(
    const QString& key, const DevicePositionExtended& value)
{
    constexpr auto queryStr =
        "UPDATE device_position "
        "SET mode=:mode, "
        "latitude=:latitude, "
        "longitude=:longitude, "
        "altitude=:altitude, "
        "azimuth=:azimuth "
        "WHERE device_id=:device_id";
    auto query = m_dbService->prepare(queryStr);

    query->bindValue(":device_id", key);
    OAService::OAServiceGeo_position_mode mode;
    mode.setValue(value.mode);
    query->bindValue(":mode", mode.asJson());
    query->bindValue(":latitude", value.position.coordinate.latitude());
    query->bindValue(":longitude", value.position.coordinate.longitude());
    query->bindValue(":altitude", value.position.coordinate.altitude());
    query->bindValue(":azimuth", value.position.azimuth);
    m_dbService->exec(*query);
}

void QueryHelper<DevicePositionExtended>::remove(const QString& key)
{
    constexpr auto queryStr =
        "DELETE FROM device_position "
        "WHERE device_id=:device_id";
    auto query = m_dbService->prepare(queryStr);

    query->bindValue(":device_id", key);
    m_dbService->exec(*query);
}

void QueryHelper<DevicePositionExtended>::clear()
{
    constexpr auto queryStr = "DELETE FROM device_position";
    m_dbService->exec(queryStr);
}

std::optional<DevicePositionExtended> QueryHelper<DevicePositionExtended>::find(
    const QString& key)
{
    constexpr auto queryStr =
        "SELECT device_id, mode, latitude, longitude, altitude, azimuth "
        "FROM device_position "
        "WHERE device_id=:device_id";
    auto query = m_dbService->prepare(queryStr);

    query->bindValue(":device_id", key);
    m_dbService->exec(*query);

    if (query->next())
    {
        return query->rowValue<DevicePositionExtended>();
    }
    return std::nullopt;
}

} // namespace mpk::dss::core
