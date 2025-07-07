#pragma once

#include "CameraDevice.h"

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class CameraApiAdapter
{
public:
    CameraApiAdapter(
        rest::ApiGateway& apiGateway,
        std::shared_ptr<CameraProviderPtrs> providers);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);
    [[nodiscard]] CameraDevice* findCamera(const DeviceId& id) const;

private:
    std::shared_ptr<CameraProviderPtrs> m_providers;
};

} // namespace mpk::dss::core
