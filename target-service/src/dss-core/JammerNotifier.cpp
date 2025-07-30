#include "JammerNotifier.h"

#include "json/JammerInfoVsJson.h"

#include "gsl/assert"

namespace mpk::dss::core
{

JammerNotifier::JammerNotifier(
    std::shared_ptr<JammerProviderPtrs> providers, QObject* parent) :
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

                connectJammer(*it);
            });
    }
}

void JammerNotifier::connectJammer(const JammerDevice& device)
{
    auto id = device.id();
    auto& properties = device.properties();

    connect(
        properties.bandSignals(),
        &mpk::dss::core::JammerBandSignals::bandsChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    connect(
        properties.disabledSignals(),
        &mpk::dss::core::DeviceDisabledSignals::disabledChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    connect(
        properties.hwInfoSignals(),
        &mpk::dss::core::DeviceHwInfoSignals::hwInfoChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    connect(
        properties.positionSignals(),
        &mpk::dss::core::DevicePositionSignals::positionChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    connect(
        properties.stateSignals(),
        &mpk::dss::core::DeviceStateSignals::stateChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    connect(
        properties.workzoneSignals(),
        &mpk::dss::core::DeviceWorkzoneSignals::workzoneChanged,
        this,
        [this, id]() { notifyJammer(id); },
        Qt::QueuedConnection);

    notifyJammer(id);
}

void JammerNotifier::notifyJammer(const DeviceId& id)
{
    auto* jammer = findDevice(id);
    Ensures(jammer);

    constexpr auto jammerMsg = "jammer_info";
    emit notify(
        jammerMsg, json::toValue(JammerInfo::fromDevice(*jammer)).toObject());
}

JammerDevice* JammerNotifier::findDevice(const DeviceId& deviceId) const
{
    for (const auto& provider: *m_providers)
    {
        auto it = std::find_if(
            provider->begin(),
            provider->end(),
            [deviceId](const auto& device) { return device.id() == deviceId; });
        if (it != provider->end())
        {
            return &(*it);
        }
    }
    return nullptr;
}

} // namespace mpk::dss::core
