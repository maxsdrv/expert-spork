#pragma once

#include "dss-core/CameraProperties.h"
#include "dss-core/Device.h"

#include "boost/iterators/ForwardIterator.h"
#include "boost/iterators/details/ValueTypeForwardIterator.h"

#include <QObject>

namespace mpk::dss::core
{

class CameraProvider
{
public:
    using Camera = Device<CameraProperties>;
    using Iterator = mpkit::ForwardIterator<Camera>;

    CameraProvider() = default;

    [[nodiscard]] Iterator begin()
    {
        return Iterator{std::make_shared<PrivateIterator>(m_devices.begin())};
    }

    [[nodiscard]] Iterator end()
    {
        return Iterator{std::make_shared<PrivateIterator>(m_devices.end())};
    }

    [[nodiscard]] Iterator find(const DeviceId& deviceId)
    {
        auto it = std::find_if(
            m_devices.begin(),
            m_devices.end(),
            [deviceId](const auto& device) { return device.id() == deviceId; });
        return Iterator{std::make_shared<PrivateIterator>(it)};
    }

    void append(
        const DeviceId& deviceId, std::unique_ptr<CameraProperties> properties)
    {
        m_devices.emplace_back(deviceId, std::move(properties));
    }

private:
    using Storage = std::vector<Camera>;
    using PrivateIterator = mpkit::details::
        ValueTypeForwardIterator<typename Storage::iterator, Camera>;

    Storage m_devices;
};

} // namespace mpk::dss::core
