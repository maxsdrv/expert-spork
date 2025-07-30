#pragma once

#include "OAServiceGeo_direction.h"

#include "dss-core/DeviceClass.h"
#include "dss-core/TrackId.h"

namespace mpk::dss::core
{

class CameraProperties
{
public:
    virtual ~CameraProperties() = default;

    [[nodiscard]] virtual QString id() const = 0;
};

} // namespace mpk::dss::core
