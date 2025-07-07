#include "TrackApiAdapter.h"

#include "TrackController.h"
#include "json/TrackVsJson.h"

#include "mpk/rest/ApiGateway.h"

#include "json/JsonParser.h"

#include "types/strong_typedef/json/JsonParser.hpp"

#include <QJsonObject>

namespace mpk::dss::core
{

TrackApiAdapter::TrackApiAdapter(
    rest::ApiGateway& apiGateway,
    gsl::not_null<const TrackController*> trackController) :
  m_trackController(trackController)
{
    registerHandlers(apiGateway);
}

void TrackApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerAsyncHandler(
        "track/subscribe",
        http::HttpMethod::POST,
        [](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO
            return {};
        });

    apiGateway.registerAsyncHandler(
        "track/unsubscribe",
        http::HttpMethod::POST,
        [](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO
            return {};
        });

    constexpr auto tracksTag = "track";
    apiGateway.registerSyncHandler(
        "track/get_tracks",
        http::HttpMethod::GET,
        [this]() -> JsonResult
        {
            std::vector<QJsonObject> tracksJson;
            std::transform(
                m_trackController->begin(),
                m_trackController->end(),
                std::back_inserter(tracksJson),
                [](const auto& trackData)
                { return json::toValue(trackData).toObject(); });
            return {{tracksTag, json::toValue(tracksJson)}};
        });

    constexpr auto trackIdTag = "track_id";
    constexpr auto trackInfoTag = "track_info";
    apiGateway.registerSyncHandler(
        "track/get_track_info",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id = json::fromObject<TrackId>(params, trackIdTag);
            auto it = m_trackController->find(id);
            if (it != m_trackController->end())
            {
                return QJsonObject{{trackInfoTag, json::toValue(*it)}};
            }

            return {};
        });
}

} // namespace mpk::dss::core
