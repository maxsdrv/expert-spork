#include "DiagnosticDataGenerator.h"

#include "diagnostic/DiagnosticController.h"

#include "log/Log2.h"

#include <array>
#include <iostream>
#include <string>

namespace
{
using Tags = mpk::dss::backend::rest::DeviceDiagnosticData::Tags;
const std::array<std::string, 2> stateList = {{Tags::ok, Tags::failure}};
const double maxCpuTemp = 95.0;
} // namespace

namespace mpk::dss
{

DiagnosticDataGenerator::DiagnosticDataGenerator(
    gsl::not_null<DiagnosticController*> diagnosticController) :
  m_diagnosticController(diagnosticController),
  m_intDist(0, stateList.size() - 1),
  m_doubleDist(0.0, maxCpuTemp),
  m_generator(m_randomDevice())
{
    generate();
}

backend::rest::DeviceDiagnosticData DiagnosticDataGenerator::generate()
{
    auto diagnostic = backend::rest::DeviceDiagnosticData{
        .state = stateList.at(m_intDist(m_generator)),
        .cpuTemp = m_doubleDist(m_generator)};
    m_diagnosticController->setCurrent(diagnostic);

    LOG_DEBUG << "[sensor-state] json:  " << diagnostic.state
              << ", cpu_temp: " << diagnostic.cpuTemp;

    return diagnostic;
}

} // namespace mpk::dss
