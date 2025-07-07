#pragma once

#include "dss-core/DeviceType.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class DeviceTypeControl
{
public:
    virtual ~DeviceTypeControl() = default;

    [[nodiscard]] virtual DeviceType::Value type() const = 0;
};

} // namespace mpk::dss::core
