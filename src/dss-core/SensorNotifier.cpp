#include "SensorNotifier.h"

#include "json/SensorVsJson.h"

#include "gsl/assert"

namespace mpk::dss::core
{

SensorNotifier::SensorNotifier(
    std::shared_ptr<SensorProviderPtrs> providers, QObject* parent) :
  QObject(parent), m_providers{std::move(providers)}
{
    for (const auto& provider: *m_providers)
    {
        QObject::connect(
            provider->pin(),
            &DeviceProviderSignals::registered,
            provider->pin(),
            [this, provider](const auto& deviceId)
            {
                auto it = provider->find(deviceId);
                Ensures(it != provider->end());

                connectSensor(*it);
            });
    }
}

void SensorNotifier::connectSensor(const SensorDevice& device)
{
    auto id = device.id();
    auto& properties = device.properties();

    connect(
        properties.disabledSignals(),
        &mpk::dss::core::DeviceDisabledSignals::disabledChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    connect(
        properties.hwInfoSignals(),
        &mpk::dss::core::DeviceHwInfoSignals::hwInfoChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    connect(
        properties.jammerSignals(),
        &mpk::dss::core::SensorJammerSignals::jammerDataChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    connect(
        properties.positionSignals(),
        &mpk::dss::core::DevicePositionSignals::positionChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    connect(
        properties.stateSignals(),
        &mpk::dss::core::DeviceStateSignals::stateChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    connect(
        properties.workzoneSignals(),
        &mpk::dss::core::DeviceWorkzoneSignals::workzoneChanged,
        this,
        [this, id]() { notifySensor(id); },
        Qt::QueuedConnection);

    notifySensor(id);
}

void SensorNotifier::notifySensor(const DeviceId& id)
{
    auto* sensor = findDevice<SensorProperties>(m_providers, id);
    Ensures(sensor);

    constexpr auto sensorMsg = "sensor_info";
    emit notify(sensorMsg, json::toValue(*sensor).toObject());
}

} // namespace mpk::dss::core
