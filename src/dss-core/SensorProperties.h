#pragma once

#include "dss-core/DeviceClass.h"
#include "dss-core/DeviceDataControl.h"
#include "dss-core/DeviceDisabledControl.h"
#include "dss-core/DeviceHwInfoControl.h"
#include "dss-core/DevicePositionControl.h"
#include "dss-core/DeviceStateControl.h"
#include "dss-core/DeviceTypeControl.h"
#include "dss-core/DeviceWorkzoneControl.h"
#include "dss-core/SensorJammerControl.h"

namespace mpk::dss::core
{

class SensorProperties : public DeviceTypeControl,
                         public DevicePositionControl,
                         public DeviceWorkzoneControl,
                         public DeviceDisabledControl,
                         public DeviceDataControl,
                         public DeviceStateControl,
                         virtual public SensorJammerControl,
                         public DeviceHwInfoControl
{
public:
    static constexpr auto deviceClass = DeviceClass::SENSOR;
};

} // namespace mpk::dss::core
