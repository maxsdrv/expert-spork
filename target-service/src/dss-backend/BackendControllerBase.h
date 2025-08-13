#pragma once

#include "dss-backend/BackendController.h"

#include "dss-core/CameraDevice.h"
#include "dss-core/JammerDevice.h"
#include "dss-core/SensorDevice.h"

#include <memory>

namespace mpk::dss::backend
{

struct BackendComponents;
class BackendSignals;

class BackendControllerBase : public BackendController
{

public:
    BackendControllerBase(
        std::unique_ptr<BackendSignals> pin,
        const BackendComponents& components);

    [[nodiscard]] core::JammerProviderPtr jammers() const override;
    [[nodiscard]] core::SensorProviderPtr sensors() const override;
    [[nodiscard]] core::CameraProviderPtr cameras() const override;
    [[nodiscard]] BackendSignals* pin() const override;
    [[nodiscard]] CameraSignals* cameraPin() const override;

protected:
    void appendJammer(std::unique_ptr<core::JammerProperties> jammer);
    void appendSensor(std::unique_ptr<core::SensorProperties> sensor);
    void appendCamera(
        const core::DeviceId& deviceId,
        std::unique_ptr<core::CameraProperties> camera);

private:
    std::unique_ptr<BackendSignals> m_pin;
    std::unique_ptr<CameraSignals> m_cameraPin;

    std::unique_ptr<core::JammerStorage> m_jammers;
    std::unique_ptr<core::SensorStorage> m_sensors;
    std::unique_ptr<core::CameraProvider> m_cameras;
};

} // namespace mpk::dss::backend
