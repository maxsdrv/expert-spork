#pragma once

#include "dss-core/JammerDevice.h"

#include "gsl/pointers"

#include <vector>

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss
{

class JammerApiAdapter
{
public:
    JammerApiAdapter(
        rest::ApiGateway& apiGateway, std::vector<core::JammerDevice> jammers);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    std::vector<core::JammerDevice> m_jammers;
};

} // namespace mpk::dss
