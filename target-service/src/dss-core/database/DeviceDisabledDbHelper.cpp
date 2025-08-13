#include "dss-core/database/DeviceDisabledDbHelper.h"
#include "dss-core/database/DbService.h"
#include "dss-core/database/DbServiceFactory.h"

#include <utility>

namespace mpk::dss::core
{

struct IdDisabledMapping
{
    QString id;
    bool disabled;
};

DeviceDisabledDbHelper::DeviceDisabledDbHelper() :
  m_dbService(createDbService())
{
}

DeviceDisabledDbHelper::DeviceDisabledDbHelper(
    std::shared_ptr<DbService> dbService) :
  m_dbService(std::move(dbService))
{
}

template <>
IdDisabledMapping fromQueryCursor<IdDisabledMapping>(QueryCursor* cursor)
{
    auto id = cursor->nextValue<QString>();
    auto disabled = cursor->nextValue<bool>();
    return {id, disabled};
}

DisabledStateList DeviceDisabledDbHelper::queryStates()
{
    auto query = m_dbService->prepare(
        "SELECT device_id, disabled "
        "FROM device_settings");

    m_dbService->exec(*query);

    DisabledStateList data;
    while (query->next())
    {
        auto row = query->rowValue<IdDisabledMapping>();
        SettingsDevice dev;
        dev.setId(row.id);
        dev.setDisabled(row.disabled);
        data.append(dev);
    }
    return data;
}

void DeviceDisabledDbHelper::saveDisabled(const DisabledStateList& dList)
{
    LOG_TRACE << "DeviceDisabledDbControl::saveDisabled(): start";
    for (const auto& device: dList)
    {
        LOG_TRACE
            << "DeviceDisabledDbControl::saveDisabled(): " << device.getId()
            << "  " << device.isDisabled();
        auto query = m_dbService->prepare(
            "INSERT INTO device_settings (device_id, disabled) "
            "VALUES(:id_val, :disabled_val)"
            "ON CONFLICT(device_id) DO UPDATE SET disabled = EXCLUDED.disabled");

        query->bindValue(":id_val", device.getId());
        query->bindValue(":disabled_val", device.isDisabled());
        m_dbService->exec(*query);
    }
}

void DeviceDisabledDbHelper::clear()
{
    m_dbService->exec("DELETE FROM device_settings");
}

}; // namespace mpk::dss::core
