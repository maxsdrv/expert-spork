#pragma once

#include "dss-backend/BackendSignals.h"
#include "dss-backend/CameraSignals.h"
#include "dss-core/CameraDevice.h"
#include "dss-core/JammerDevice.h"
#include "dss-core/SensorDevice.h"

#include <memory>

namespace mpk::dss::backend
{

class BackendSignals;

class BackendController
{

public:
    virtual ~BackendController() = default;

    [[nodiscard]] virtual core::JammerProviderPtr jammers() const = 0;
    [[nodiscard]] virtual core::SensorProviderPtr sensors() const = 0;
    [[nodiscard]] virtual core::CameraProviderPtr cameras() const = 0;

    [[nodiscard]] virtual BackendSignals* pin() const = 0;
    [[nodiscard]] virtual CameraSignals* cameraPin() const = 0;
};

using BackendControllerPtr = std::unique_ptr<BackendController>;

} // namespace mpk::dss::backend
