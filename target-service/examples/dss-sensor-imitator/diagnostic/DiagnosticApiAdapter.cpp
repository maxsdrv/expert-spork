#include "diagnostic/DiagnosticApiAdapter.h"

#include "diagnostic/DiagnosticController.h"

#include "dss-backend/rest/json/DiagnosticDataVsJson.h"

#include "mpk/rest/ApiGateway.h"

namespace mpk::dss
{

DiagnosticApiAdapter::DiagnosticApiAdapter(
    rest::ApiGateway& apiGateway,
    gsl::not_null<DiagnosticController*> controller) :
  m_controller(controller)
{
    registerHandlers(apiGateway);
}

void DiagnosticApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    apiGateway.registerSyncHandler(
        "get_diagnostic_data",
        [this]()
        {
            auto diagnostic = m_controller->current();
            return json::toValue(diagnostic).toObject();
        });
}

} // namespace mpk::dss
