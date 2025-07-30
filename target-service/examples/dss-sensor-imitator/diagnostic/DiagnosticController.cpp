#include "DiagnosticController.h"

#include <utility>

namespace mpk::dss
{

DiagnosticController::DiagnosticController(
    backend::rest::DeviceDiagnosticData diagnostic) :
  m_diagnostic(std::move(diagnostic))
{
}

void DiagnosticController::setCurrent(
    const backend::rest::DeviceDiagnosticData& diagnostic)
{
    m_diagnostic = diagnostic;
}

backend::rest::DeviceDiagnosticData DiagnosticController::current() const
{
    return m_diagnostic;
}

} // namespace mpk::dss
