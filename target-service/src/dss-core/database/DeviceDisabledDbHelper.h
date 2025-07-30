#pragma once

#include "dss-core/DeviceId.h"
#include "dss-core/DevicesDisabledSet.h"

namespace mpk::dss::core
{

class DbService;

class DeviceDisabledDbHelper
{
public:
    DeviceDisabledDbHelper();
    explicit DeviceDisabledDbHelper(std::shared_ptr<DbService>);
    DisabledStateList queryStates();
    void saveDisabled(const DisabledStateList&);
    void clear();

private:
    std::shared_ptr<DbService> m_dbService;
};

} // namespace mpk::dss::core
