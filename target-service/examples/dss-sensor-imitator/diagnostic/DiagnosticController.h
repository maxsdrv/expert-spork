#pragma once

#include "dss-backend/rest/diagnostic/DeviceDiagnosticData.h"

namespace mpk::dss
{

class DiagnosticController
{
public:
    explicit DiagnosticController(
        backend::rest::DeviceDiagnosticData diagnostic);

    void setCurrent(const backend::rest::DeviceDiagnosticData& diagnostic);
    [[nodiscard]] backend::rest::DeviceDiagnosticData current() const;

private:
    backend::rest::DeviceDiagnosticData m_diagnostic;
};

} // namespace mpk::dss
