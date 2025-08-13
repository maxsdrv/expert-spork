#pragma once

#include "dss-core/DeviceHwInfo.h"
#include "dss-core/DeviceHwInfoSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DeviceHwInfoControl
{
public:
    virtual ~DeviceHwInfoControl() = default;

    [[nodiscard]] virtual DeviceHwInfo hwInfo() const = 0;
    [[nodiscard]] virtual gsl::not_null<const DeviceHwInfoSignals*> hwInfoSignals()
        const = 0;
};

} // namespace mpk::dss::core
