#pragma once

#include "DevicePositionStorage.h"
#include "JammerDevice.h"
#include "SensorDevice.h"

#include "OAServiceGeo_position.h"

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class DeviceApiAdapter
{
public:
    DeviceApiAdapter(
        rest::ApiGateway& apiGateway,
        std::shared_ptr<SensorProviderPtrs> sensorProviders,
        std::shared_ptr<JammerProviderPtrs> jammerProviders);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    std::shared_ptr<SensorProviderPtrs> m_sensorProviders;
    std::shared_ptr<JammerProviderPtrs> m_jammerProviders;
};

} // namespace mpk::dss::core
