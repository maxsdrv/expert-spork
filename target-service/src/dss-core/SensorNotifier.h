#pragma once

#include "dss-core/SensorDevice.h"

#include <QObject>

namespace mpk::dss::core
{

template <
    class Properties,
    class ProviderPtrs = std::vector<gsl::not_null<DeviceProvider<Properties>*>>>
Device<Properties>* findDevice(ProviderPtrs& providers, const DeviceId& deviceId)
{
    for (const auto& provider: *providers)
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

class SensorNotifier : public QObject
{
    Q_OBJECT

public:
    explicit SensorNotifier(
        std::shared_ptr<SensorProviderPtrs> providers,
        QObject* parent = nullptr);

private:
    void connectSensor(const SensorDevice& device);

private slots:
    void notifySensor(const DeviceId& id);

signals:
    void notify(QString, QJsonObject);

private:
    std::shared_ptr<SensorProviderPtrs> m_providers;
};

} // namespace mpk::dss::core
