#pragma once

#include "dss-core/DeviceWorkzoneSignals.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DeviceWorkzoneControl
{
public:
    virtual ~DeviceWorkzoneControl() = default;

    [[nodiscard]] virtual DeviceWorkzone workzone() const = 0;
    [[nodiscard]] virtual gsl::not_null<const DeviceWorkzoneSignals*>
    workzoneSignals() const = 0;
};

} // namespace mpk::dss::core
