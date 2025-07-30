#pragma once

#include "DeviceDisabledSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DeviceDisabledControl
{
public:
    virtual ~DeviceDisabledControl() = default;

    [[nodiscard]] virtual bool disabled() const = 0;
    virtual void setDisabled(bool disabled) = 0;

    [[nodiscard]] virtual gsl::not_null<const DeviceDisabledSignals*>
    disabledSignals() const = 0;
};

} // namespace mpk::dss::core
