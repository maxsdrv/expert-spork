#pragma once

#include "gsl/pointers"

namespace mpk::rest
{

class ApiGateway;

} // namespace mpk::rest

namespace mpk::dss
{

class DiagnosticController;

class DiagnosticApiAdapter
{
public:
    DiagnosticApiAdapter(
        rest::ApiGateway& apiGateway,
        gsl::not_null<DiagnosticController*> controller);

private:
    void registerHandlers(rest::ApiGateway& apiGateway);

private:
    gsl::not_null<DiagnosticController*> m_controller;
};

} // namespace mpk::dss
