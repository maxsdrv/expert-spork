#pragma once

#include "dss-core/SensorDevice.h"

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class SensorApiAdapter
{
public:
    SensorApiAdapter(
        rest::ApiGateway& apiGateway,
        std::shared_ptr<SensorProviderPtrs> providers);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

    [[nodiscard]] SensorDevice* findDevice(const DeviceId& deviceId) const;

private:
    std::shared_ptr<SensorProviderPtrs> m_providers;
};

} // namespace mpk::dss::core
