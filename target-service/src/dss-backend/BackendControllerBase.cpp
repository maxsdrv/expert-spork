#include "BackendControllerBase.h"

#include "BackendComponents.h"
#include "BackendSignals.h"

namespace mpk::dss::backend
{

BackendControllerBase::BackendControllerBase(
    std::unique_ptr<BackendSignals> pin, const BackendComponents& components) :
  m_pin(std::move(pin)),
  m_cameraPin(std::make_unique<CameraSignals>()),
  m_jammers{std::make_unique<core::JammerStorage>(
      components.deviceIdMappingStorage, components.devicePositionStorage)},
  m_sensors{std::make_unique<core::SensorStorage>(
      components.deviceIdMappingStorage, components.devicePositionStorage)},
  m_cameras{std::make_unique<core::CameraProvider>()}
{
}

auto BackendControllerBase::jammers() const -> core::JammerProviderPtr
{
    return m_jammers.get();
}

auto BackendControllerBase::sensors() const -> core::SensorProviderPtr
{
    return m_sensors.get();
}

auto BackendControllerBase::cameras() const -> core::CameraProviderPtr
{
    return m_cameras.get();
}

BackendSignals* BackendControllerBase::pin() const
{
    return m_pin.get();
}

CameraSignals* BackendControllerBase::cameraPin() const
{
    return m_cameraPin.get();
}

void BackendControllerBase::appendSensor(
    std::unique_ptr<core::SensorProperties> sensor)
{
    m_sensors->append(std::move(sensor));
}

void BackendControllerBase::appendJammer(
    std::unique_ptr<core::JammerProperties> jammer)
{
    m_jammers->append(std::move(jammer));
}

void BackendControllerBase::appendCamera(
    const core::DeviceId& deviceId,
    std::unique_ptr<core::CameraProperties> camera)
{
    m_cameras->append(deviceId, std::move(camera));
}

} // namespace mpk::dss::backend
