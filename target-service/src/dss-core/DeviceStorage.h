#pragma once

#include "dss-core/DevicePositionStorage.h"
#include "dss-core/DeviceProvider.h"
#include "dss-core/DeviceRegistrator.h"

#include <QObject>

namespace mpk::dss::core
{

template <typename Properties>
class DeviceStorage : public DeviceProvider<Properties>
{
public:
    explicit DeviceStorage(
        std::shared_ptr<DeviceIdMappingStorage> deviceIdMapping,
        std::shared_ptr<DevicePositionStorage> devicePositionStorage) :
      m_deviceIdMapping{std::move(deviceIdMapping)},
      m_devicePositionStorage{std::move(devicePositionStorage)}
    {
    }

    void append(std::unique_ptr<Properties> properties)
    {
        auto* registrator = new DeviceRegistrator(
            m_deviceIdMapping, properties.get(), Properties::deviceClass);
        QObject::connect(
            registrator,
            &DeviceRegistrator::registered,
            registrator,
            [this, registrator, properties = std::move(properties)](
                const auto& deviceId) mutable
            {
                syncDevicePosition(deviceId, properties.get());
                this->appendRegistered(deviceId, std::move(properties));
                registrator->deleteLater();
            });
        QObject::connect(
            registrator,
            &DeviceRegistrator::failed,
            registrator,
            [registrator]() { registrator->deleteLater(); });
    }

private:
    void syncDevicePosition(const DeviceId& deviceId, Properties* properties)
    {
        auto position =
            m_devicePositionStorage->find(static_cast<QString>(deviceId));
        if (position)
        {
            properties->setPositionMode(position->mode);
            if (position->mode != DevicePositionMode::AUTO)
            {
                properties->setPosition(position->position);
            }
        }
        else
        {
            m_devicePositionStorage->modifier()->append(DevicePositionExtended{
                .key = static_cast<QString>(deviceId),
                .mode = properties->positionMode(),
                .position = properties->position()});
        }
    }

private:
    std::shared_ptr<DeviceIdMappingStorage> m_deviceIdMapping;
    std::shared_ptr<DevicePositionStorage> m_devicePositionStorage;
};

} // namespace mpk::dss::core
