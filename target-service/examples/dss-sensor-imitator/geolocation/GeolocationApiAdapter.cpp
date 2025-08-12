#include "GeolocationApiAdapter.h"

#include "geolocation/GeolocationController.h"

#include "dss-backend/rest/json/GeolocationVsJson.h"

#include "mpk/rest/ApiGateway.h"

namespace mpk::dss
{

GeolocationApiAdapter::GeolocationApiAdapter(
    rest::ApiGateway& apiGateway,
    gsl::not_null<GeolocationController*> controller) :
  m_controller(controller)
{
    registerHandlers(apiGateway);
}

void GeolocationApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    apiGateway.registerSyncHandler(
        "get_geolocation",
        [this]()
        {
            auto location = m_controller->current();
            return json::toValue(location).toObject();
        });

    apiGateway.registerAsyncHandler(
        "set_geolocation",
        [this](const QJsonObject& json)
        {
            auto location = json::fromValue<backend::rest::Geolocation>(json);
            m_controller->setCurrent(location);
        });
}

} // namespace mpk::dss
