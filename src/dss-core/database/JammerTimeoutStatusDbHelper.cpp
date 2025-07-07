#include "dss-core/database/JammerTimeoutStatusDbHelper.h"
#include "dss-core/JammerTimeoutStatus.h"
#include "dss-core/database/DbServiceFactory.h"

namespace mpk::dss::core
{

JammerTimeoutStatusDbHelper::JammerTimeoutStatusDbHelper() :
  m_dbService(createDbService())
{
}

JammerTimeoutStatusDbHelper::JammerTimeoutStatusDbHelper(
    std::shared_ptr<DbService> dbService) :
  m_dbService(std::move(dbService))
{
}

struct JammerTimeoutStatusDb : public JammerTimeoutStatus
{
    QString deviceId;
};

template <>
JammerTimeoutStatusDb fromQueryCursor<JammerTimeoutStatusDb>(QueryCursor* cursor)
{
    JammerTimeoutStatusDb status;
    status.deviceId = cursor->nextValue<QString>();
    auto startedTime =
        QDateTime::fromString(cursor->nextValue<QString>(), Qt::ISODate)
            .toSecsSinceEpoch();
    status.timeoutSec = cursor->nextValue<int>();
    status.started = std::chrono::system_clock::from_time_t(startedTime);
    status.now = std::chrono::system_clock::now();
    return status;
}

void JammerTimeoutStatusDbHelper::saveStatus(
    const QString& deviceId, const JammerTimeoutStatus& status)
{
    // should clear bands
    clearStatus(deviceId);

    auto query = m_dbService->prepare(
        "INSERT INTO jammer_timeout_status (device_id, started_at, timeout) "
        "VALUES(:id_val, :started_val, :timeout_val)");

    auto startedTime = QDateTime::fromSecsSinceEpoch(
                           std::chrono::system_clock::to_time_t(status.started))
                           .toString(Qt::ISODate);

    query->bindValue(":id_val", deviceId);
    query->bindValue(":started_val", startedTime);
    query->bindValue(":timeout_val", status.timeoutSec);

    m_dbService->exec(*query);

    if (!status.bands.empty())
    {
        auto bandsQuery = m_dbService->prepare(
            "INSERT INTO jammer_bands_active (device_id, band) "
            "VALUES(:id_val, :band_val) ");

        for (const auto& bandLabel: status.bands)
        {
            bandsQuery->bindValue(":id_val", deviceId);
            bandsQuery->bindValue(":band_val", bandLabel);
            m_dbService->exec(*bandsQuery);
        }
    }
}

void JammerTimeoutStatusDbHelper::saveMode(
    const QString& deviceId,
    const JammerMode::Value& mode,
    std::chrono::seconds timeout)
{
    auto query = m_dbService->prepare(
        "INSERT INTO jammer_mode (sensor_id, mode, timeout) "
        "VALUES(:id_val, :mode_val, :timeout_val) "
        "ON CONFLICT(sensor_id) DO UPDATE SET "
        "mode = EXCLUDED.mode, "
        "timeout = EXCLUDED.timeout");

    query->bindValue(":id_val", deviceId);
    query->bindValue(":mode_val", JammerMode::toString(mode));
    query->bindValue(":timeout_val", QVariant::fromValue(timeout.count()));

    m_dbService->exec(*query);
}

void JammerTimeoutStatusDbHelper::clearStatus(const QString& deviceId)
{
    auto statusQuery = m_dbService->prepare(
        "DELETE FROM jammer_timeout_status WHERE device_id = :id_val");
    statusQuery->bindValue(":id_val", deviceId);
    m_dbService->exec(*statusQuery);
}

bool JammerTimeoutStatusDbHelper::queryStatus(
    const QString& deviceId, JammerTimeoutStatus& status) const
{
    auto query = m_dbService->prepare(
        "SELECT device_id, started_at, timeout "
        "FROM jammer_timeout_status "
        "WHERE device_id = :id_val");

    query->bindValue(":id_val", deviceId);
    m_dbService->exec(*query);

    if (query->next())
    {
        auto dbStatus = query->rowValue<JammerTimeoutStatusDb>();
        status.started = dbStatus.started;
        status.now = dbStatus.now;
        status.timeoutSec = dbStatus.timeoutSec;

        auto bandsQuery = m_dbService->prepare(
            "SELECT band FROM jammer_bands_active WHERE device_id = :id_val");
        bandsQuery->bindValue(":id_val", dbStatus.deviceId);
        m_dbService->exec(*bandsQuery);

        while (bandsQuery->next())
        {
            status.bands.push_back(bandsQuery->nextValue<QString>());
        }
        return true;
    }
    return false;
}

bool JammerTimeoutStatusDbHelper::queryMode(
    const QString& deviceId,
    JammerMode::Value& mode,
    std::chrono::seconds& timeout) const
{
    auto query = m_dbService->prepare(
        "SELECT mode, timeout FROM jammer_mode WHERE sensor_id = :id_val");

    query->bindValue(":id_val", deviceId);
    m_dbService->exec(*query);

    if (query->next())
    {
        auto modeStr = query->nextValue<QString>();
        auto timeoutSecs = query->nextValue<int>();
        mode = JammerMode::fromString(modeStr);
        timeout = std::chrono::seconds(timeoutSecs);
        return true;
    }

    mode = JammerMode::Value::UNDEFINED;
    timeout = std::chrono::seconds(0);
    return false;
}

} // namespace mpk::dss::core
