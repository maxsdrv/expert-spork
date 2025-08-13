#pragma once

#include "dss-core/JammerDevice.h"
#include "dss-core/JammerGroupProvider.h"
#include "dss-core/WsPublisher.h"

#include <memory>

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class JammerApiAdapter
{
public:
    JammerApiAdapter(
        rest::ApiGateway& apiGateway,
        std::shared_ptr<JammerProviderPtrs> providers,
        gsl::shared_ptr<JammerGroupProvider> groupProvider,
        std::shared_ptr<WsPublisher> wsPublisher);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

    [[nodiscard]] JammerDevice* findDevice(const DeviceId& deviceId) const;

private:
    std::shared_ptr<JammerProviderPtrs> m_providers;
    gsl::shared_ptr<JammerGroupProvider> m_groupProvider;
    std::shared_ptr<WsPublisher> m_wsPublisher;
};

} // namespace mpk::dss::core
