#pragma once

#include "dss-core/DeviceClass.h"
#include "dss-core/DeviceDataControl.h"
#include "dss-core/DeviceDisabledControl.h"
#include "dss-core/DeviceHwInfoControl.h"
#include "dss-core/DeviceId.h"
#include "dss-core/DevicePositionControl.h"
#include "dss-core/DeviceStateControl.h"
#include "dss-core/DeviceWorkzoneControl.h"
#include "dss-core/JammerBandControl.h"
#include "dss-core/JammerGroupControl.h"
#include "dss-core/JammerTimeoutDeviceControl.h"

namespace mpk::dss::core
{

class JammerProperties : public DevicePositionControl,
                         public DeviceWorkzoneControl,
                         public DeviceDataControl,
                         public DeviceDisabledControl,
                         public DeviceStateControl,
                         public JammerBandControl,
                         public virtual JammerTimeoutDeviceControl,
                         public JammerGroupControl,
                         public DeviceHwInfoControl
{
public:
    static constexpr auto deviceClass = DeviceClass::JAMMER;

    [[nodiscard]] virtual DeviceId sensorId() const = 0;
};

} // namespace mpk::dss::core
