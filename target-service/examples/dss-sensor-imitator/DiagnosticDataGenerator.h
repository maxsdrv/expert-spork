#pragma once

#include "dss-backend/rest/diagnostic/DeviceDiagnosticData.h"

#include "gsl/pointers"

#include <random>

namespace mpk::dss
{

class DiagnosticController;

class DiagnosticDataGenerator
{
public:
    explicit DiagnosticDataGenerator(
        gsl::not_null<DiagnosticController*> diagnosticController);

    backend::rest::DeviceDiagnosticData generate();

private:
    gsl::not_null<DiagnosticController*> m_diagnosticController;

    std::uniform_int_distribution<int> m_intDist;
    std::uniform_real_distribution<double> m_doubleDist;
    std::random_device m_randomDevice;
    mutable std::mt19937 m_generator;
};

}; // namespace mpk::dss
// namespace mpk::dss
