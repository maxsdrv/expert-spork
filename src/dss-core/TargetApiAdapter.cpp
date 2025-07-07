#include "TargetApiAdapter.h"

#include "TargetController.h"
#include "json/TargetDataVsJson.h"

#include "mpk/rest/ApiGateway.h"

#include "json/JsonParser.h"

#include "types/strong_typedef/json/JsonParser.hpp"

#include <QJsonObject>

namespace mpk::dss::core
{

TargetApiAdapter::TargetApiAdapter(
    rest::ApiGateway& apiGateway,
    gsl::not_null<const TargetController*> targetController) :
  m_targetController(targetController)
{
    registerHandlers(apiGateway);
}

void TargetApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerAsyncHandler(
        "target/subscribe",
        http::HttpMethod::POST,
        [](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO
            return {};
        });

    apiGateway.registerAsyncHandler(
        "target/unsubscribe",
        http::HttpMethod::POST,
        [](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO
            return {};
        });

    constexpr auto targetsTag = "target";
    apiGateway.registerSyncHandler(
        "target/get_targets",
        http::HttpMethod::GET,
        [this]() -> JsonResult
        {
            std::vector<QJsonObject> targetsJson;
            std::transform(
                m_targetController->begin(),
                m_targetController->end(),
                std::back_inserter(targetsJson),
                [](const auto& targetData)
                { return json::toValue(targetData).toObject(); });
            return {{targetsTag, json::toValue(targetsJson)}};
        });

    constexpr auto targetIdTag = "target_id";
    constexpr auto targetDataTag = "target_data";
    apiGateway.registerSyncHandler(
        "target/get_target_data",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id = json::fromObject<TargetId>(params, targetIdTag);
            auto it = m_targetController->find(id);
            if (it != m_targetController->end())
            {
                return QJsonObject{{targetDataTag, json::toValue(*it)}};
            }

            return {};
        });
}

} // namespace mpk::dss::core
