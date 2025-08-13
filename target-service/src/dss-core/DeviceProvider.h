#pragma once

#include "dss-core/Device.h"
#include "dss-core/DeviceProviderSignals.h"

#include "boost/iterators/ForwardIterator.h"
#include "boost/iterators/details/ValueTypeForwardIterator.h"

#include <QObject>

namespace mpk::dss::core
{

template <typename Properties>
class DeviceProvider
{
public:
    using DeviceValue = Device<Properties>;
    using Iterator = mpkit::ForwardIterator<DeviceValue>;

    DeviceProvider() : m_pin{std::make_unique<DeviceProviderSignals>()}
    {
    }

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

    [[nodiscard]] gsl::not_null<DeviceProviderSignals*> pin() const
    {
        return m_pin.get();
    }

protected:
    void appendRegistered(
        const DeviceId& deviceId, std::unique_ptr<Properties> properties)
    {
        m_devices.emplace_back(deviceId, std::move(properties));
        emit m_pin->registered(deviceId);
    }

private:
    using Storage = std::vector<DeviceValue>;
    using PrivateIterator = mpkit::details::
        ValueTypeForwardIterator<typename Storage::iterator, DeviceValue>;

    Storage m_devices;
    std::unique_ptr<DeviceProviderSignals> m_pin;
};

} // namespace mpk::dss::core
