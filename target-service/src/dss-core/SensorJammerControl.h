#pragma once

#include "dss-core/DeviceId.h"
#include "dss-core/JammerAutoDefenseControl.h"
#include "dss-core/JammerMode.h"
#include "dss-core/LocalLock.h"
#include "dss-core/SensorJammerSignals.h"

#include "log/Log2.h"

#include "gsl/pointers"

#include <atomic>
#include <chrono>
#include <memory>
#include <optional>

namespace mpk::dss::core
{

class LicenseController;

class SensorJammerControl
{
public:
    virtual ~SensorJammerControl() = default;

    explicit SensorJammerControl(
        JammerAutoDefenseData autoData,
        gsl::not_null<const LicenseController*> licenseController) :
      m_autoData(autoData),
      m_licenseController(licenseController),
      m_jammerSignals{std::make_unique<core::SensorJammerSignals>()}
    {
        // for logging only
        setJammerMode(core::JammerMode::MANUAL, seconds(0));
    }

    SensorJammerControl() :
      m_jammerSignals{std::make_unique<core::SensorJammerSignals>()}
    { /*zombie mode for not using backends*/
    }

    [[nodiscard]] core::JammerMode::Value jammerMode() const
    {
        if (!m_autoData.has_value())
        {
            return core::JammerMode::UNDEFINED;
        }

        /*** may run in server thread ***/
        return m_autoDefense.load() ? core::JammerMode::AUTO :
                                      core::JammerMode::MANUAL;
    }

    void setJammerMode(core::JammerMode::Value jammerModeArg, seconds timeout)
    {
        if (!m_autoData.has_value())
        {
            LOG_WARNING
                << "SensorJammerControl::setJammerMode() not initialized for jammer control";
            return;
        }

        LOG_DEBUG << "SensorJammerControl::setJammerMode() jammerMode == "
                  << core::JammerMode::toString(jammerModeArg)
                  << " timeout == " << timeout;
        m_jammerTimeout = timeoutOrMaxTimeout(m_autoData->jammers, timeout);

        delete m_autoDefense.load();
        m_autoDefense.store(nullptr);

        if (jammerModeArg == core::JammerMode::AUTO)
        {
            m_autoDefense.store(new JammerAutoDefenseControl(
                m_autoData.value(), m_jammerTimeout, m_licenseController));
        }

        m_jammerSignals->emitJammerDataChanged();
    }

    [[nodiscard]] std::chrono::seconds jammerAutoTimeout() const
    {
        return m_jammerTimeout;
    }

    [[nodiscard]] virtual std::vector<DeviceId> jammerIds() const
    {
        return {};
    }

    [[nodiscard]] virtual gsl::not_null<const SensorJammerSignals*> jammerSignals()
    {
        return m_jammerSignals.get();
    }

private:
    std::chrono::seconds m_jammerTimeout{16s};
    std::optional<JammerAutoDefenseData> m_autoData;
    std::atomic<JammerAutoDefenseControl*> m_autoDefense;
    const LicenseController* m_licenseController = nullptr;
    std::unique_ptr<core::SensorJammerSignals> m_jammerSignals;
};

} // namespace mpk::dss::core
