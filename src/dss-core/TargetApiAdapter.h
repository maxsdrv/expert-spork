#pragma once

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class TargetController;

class TargetApiAdapter
{
public:
    TargetApiAdapter(
        rest::ApiGateway& apiGateway,
        gsl::not_null<const TargetController*> targetController);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    gsl::not_null<const TargetController*> m_targetController;
};

} // namespace mpk::dss::core
