#pragma once

#include "dss-core/DeviceState.h"
#include "dss-core/DeviceStateSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DeviceStateControl
{
public:
    virtual ~DeviceStateControl() = default;

    [[nodiscard]] virtual DeviceState::Value state() const = 0;
    [[nodiscard]] virtual gsl::not_null<const DeviceStateSignals*> stateSignals()
        const = 0;
};

} // namespace mpk::dss::core
