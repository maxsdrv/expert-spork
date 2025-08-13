#pragma once

#include "dss-core/DevicePosition.h"
#include "dss-core/DevicePositionSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DevicePositionControl
{
public:
    virtual ~DevicePositionControl() = default;

    [[nodiscard]] virtual DevicePosition position() const = 0;
    virtual void setPosition(const DevicePosition& position) = 0;

    [[nodiscard]] virtual DevicePositionMode positionMode() const = 0;
    virtual void setPositionMode(const DevicePositionMode& mode) = 0;

    [[nodiscard]] virtual gsl::not_null<const DevicePositionSignals*>
    positionSignals() const = 0;
};

} // namespace mpk::dss::core
