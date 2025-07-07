#pragma once

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss::core
{

class TrackController;

class TrackApiAdapter
{
public:
    TrackApiAdapter(
        rest::ApiGateway& apiGateway,
        gsl::not_null<const TrackController*> trackController);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    gsl::not_null<const TrackController*> m_trackController;
};

} // namespace mpk::dss::core
