#pragma once

#include "dss-core/JammerDevice.h"
#include "dss-core/SensorDevice.h"

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class LicenseController;

class CommonApiAdapter
{
public:
    explicit CommonApiAdapter(
        rest::ApiGateway& apiGateway,
        gsl::not_null<LicenseController*> licenseController,
        std::shared_ptr<SensorProviderPtrs> sensorProviders,
        std::shared_ptr<JammerProviderPtrs> jammerProviders);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    gsl::not_null<LicenseController*> m_licenseController;
    std::shared_ptr<SensorProviderPtrs> m_sensorProviders;
    std::shared_ptr<JammerProviderPtrs> m_jammerProviders;
};

} // namespace mpk::dss::core
