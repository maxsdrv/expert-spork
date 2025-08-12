#pragma once

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss
{

class GeolocationController;

class GeolocationApiAdapter
{
public:
    GeolocationApiAdapter(
        rest::ApiGateway& apiGateway,
        gsl::not_null<GeolocationController*> controller);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    gsl::not_null<GeolocationController*> m_controller;
};

} // namespace mpk::dss
