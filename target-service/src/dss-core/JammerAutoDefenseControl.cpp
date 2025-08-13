
#include "dss-core/JammerAutoDefenseControl.h"
#include "dss-core/JammerBandOption.h"
#include "dss-core/SensorJammerControl.h"
#include "dss-core/license/LicenseController.h"

#include "log/Log2.h"

#include <algorithm>

namespace mpk::dss::core
{

seconds timeoutOrMaxTimeout(JammerProviderPtr jammers, seconds timeout)
{
    // check against maximum for each jammer
    auto timeoutMax = [](const JammerDevice& d)
    { return d.properties().timeoutControl().timeoutMax(); };

    auto ij = std::min_element(
        jammers->begin(),
        jammers->end(),
        [=](const JammerDevice& l, const JammerDevice& r)
        { return timeoutMax(l) < timeoutMax(r); });
    if (ij == jammers->end())
    {
        return timeout;
    }
    return std::min(timeout, seconds(timeoutMax(*ij)));
}

JammerAutoDefenseControl::JammerAutoDefenseControl(
    JammerAutoDefenseData data,
    seconds& timeout,
    const LicenseController* licenseController,
    QObject* parent) :
  m_jammerProvider(data.jammers),
  m_timeout(timeout),
  m_licenseController(licenseController)
{
    Q_UNUSED(parent);
    connect(
        data.bSignals,
        &mpk::dss::backend::BackendSignals::trackUpdated,
        this,
        &mpk::dss::core::JammerAutoDefenseControl::handleTargetUpdated);

    if (m_jammerProvider->begin() == m_jammerProvider->end())
    {
        LOG_WARNING << "JammerAutoDefenseControl: no jammers available\n\n";
    }
}

void JammerAutoDefenseControl::handleTargetUpdated(
    const core::DeviceId& devId,
    const core::TrackId& trackId,
    const core::AlarmStatus& alarmSt,
    core::TargetClass::Value targetClass)
{
    Q_UNUSED(devId);
    Q_UNUSED(trackId);
    Q_UNUSED(alarmSt);
    LOG_DEBUG << "JammerAutoDefenseControl::handleTargetUpdated()";

    if ((m_licenseController == nullptr
         || m_licenseController->status().isRValid())
        && core::TargetClass::DRONE == targetClass)
    {
        for (auto ij = m_jammerProvider->begin(); ij != m_jammerProvider->end();
             ++ij)
        {
            auto* p = &ij->properties();
            if (auto bands = p->maxBands(); !bands.empty())
            {
                if (!p->timeoutControl().status())
                {
                    p->timeoutControl().jammerStartStop(
                        bands, static_cast<int>(m_timeout.count()));
                }
            }
        }
    }
}

} // namespace mpk::dss::core
