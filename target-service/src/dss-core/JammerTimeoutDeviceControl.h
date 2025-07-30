#pragma once

#include "dss-core/JammerMode.h"
#include "dss-core/JammerTimeoutControl.h"
#include "dss-core/JammerTimeoutPersistent.h"

#include <memory>

namespace mpk::dss::core
{

class SensorJammerControl;

class JammerTimeoutDeviceControl
{
public:
    explicit JammerTimeoutDeviceControl(JammerTimeoutControl* ctrl) :
      m_timeoutControl(ctrl),
      m_timeoutPersistent(new JammerTimeoutPersistent(*m_timeoutControl))
    {
    }

    [[nodiscard]] JammerTimeoutControl& timeoutControl() const
    {
        return *m_timeoutControl;
    }
    [[nodiscard]] JammerTimeoutPersistent& timeoutPersistent() const
    {
        return *m_timeoutPersistent;
    }

    virtual ~JammerTimeoutDeviceControl() = default;

private:
    std::unique_ptr<JammerTimeoutControl> m_timeoutControl;
    std::unique_ptr<JammerTimeoutPersistent> m_timeoutPersistent;
};

} // namespace mpk::dss::core
