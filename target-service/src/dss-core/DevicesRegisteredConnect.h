#pragma once

#include "dss-core/DeviceProvider.h"

namespace mpk::dss::core
{

template <class ProviderPtrs, class CallbackFunc>
void devicesRegisteredConnect(
    std::shared_ptr<ProviderPtrs> providers, CallbackFunc callbackFunction)
{
    for (const auto& provider: *providers)
    {
        QObject::connect(
            provider->pin(),
            &DeviceProviderSignals::registered,
            provider->pin(),
            [callbackFunction, provider](const auto& deviceId)
            {
                auto it = provider->find(deviceId);
                Ensures(it != provider->end());

                callbackFunction(*it);
            });
    }
};

} // namespace mpk::dss::core
