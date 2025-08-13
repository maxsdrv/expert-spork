#pragma once

#include "dss-core/JammerMode.h"

#include <memory>

namespace mpk::dss::core
{

class DbService;
struct JammerTimeoutStatus;

class JammerTimeoutStatusDbHelper
{
public:
    JammerTimeoutStatusDbHelper();
    explicit JammerTimeoutStatusDbHelper(std::shared_ptr<DbService>);

    void saveStatus(const QString& deviceId, const JammerTimeoutStatus&);
    void saveMode(
        const QString& deviceId,
        const JammerMode::Value&,
        std::chrono::seconds timeout);
    [[nodiscard]] bool queryMode(
        const QString& deviceId,
        JammerMode::Value& mode,
        std::chrono::seconds& timeout) const;
    void clearStatus(const QString& deviceId);
    [[nodiscard]] bool queryStatus(
        const QString& deviceId, JammerTimeoutStatus& status) const;

private:
    std::shared_ptr<DbService> m_dbService;
};

} // namespace mpk::dss::core
