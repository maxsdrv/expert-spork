#include "dss-core/JammerTimeoutPersistent.h"
#include "dss-common/api/Helpers.h"

#include "log/Log2.h"

namespace mpk::dss::core
{

JammerTimeoutPersistent::JammerTimeoutPersistent(
    JammerTimeoutControl& timeoutControl) :
  m_timeoutControl(&timeoutControl)
{
    connect(
        m_timeoutControl,
        &JammerTimeoutControl::statusEvent,
        this,
        &JammerTimeoutPersistent::updateStatus);
}

void JammerTimeoutPersistent::load(DeviceId id)
{
    m_deviceId = static_cast<QString>(id);
    JammerTimeoutStatus status;
    if (m_dbHelper.queryStatus(m_deviceId, status))
    {
        auto secsLeft =
            status.timeoutSec
            - static_cast<int>(
                std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now() - status.started)
                    .count());
        m_dbHelper.clearStatus(m_deviceId);
        if (secsLeft >= 0)
        {
            LOG_INFO << "JammerTimeoutPersistent: jammer " << m_deviceId
                     << " started " << api::timeToStdString(status.started)
                     << " with timeout " << status.timeoutSec
                     << "s still has more " << secsLeft << "s to run";
            m_timeoutControl->jammerStartStop(status.bands, secsLeft);
        }
        else
        {
            LOG_DEBUG
                << "JammerTimeoutPersistent: cleared expired jammer status for "
                << m_deviceId;
        }
    }
}

void JammerTimeoutPersistent::updateStatus(
    JammerTimeoutControl::StatusEvent event)
{
    if (m_deviceId.isEmpty())
    {
        LOG_ERROR << "JammerTimeoutPersistent: needs to load deviceId first";
        return;
    }
    auto status = m_timeoutControl->status();
    if (JammerTimeoutControl::STARTED == event && status)
    {
        m_dbHelper.saveStatus(m_deviceId, *status);
    }
    else
    {
        m_dbHelper.clearStatus(m_deviceId);
    }
}

} // namespace mpk::dss::core
