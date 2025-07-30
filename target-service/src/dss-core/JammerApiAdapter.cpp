#include "JammerApiAdapter.h"

#include "JammerBand.h"
#include "JammerDevice.h"
#include "JammerInfo.h"
#include "JammerProperties.h"
#include "WsReply.h"
#include "json/JammerInfoVsJson.h"

#include "OAServiceSet_jammer_bands_request.h"

#include "httpws/HttpException.h"
#include "httpws/HttpMethod.h"
#include "json/FromJson.h"
#include "json/ToJson.h"
#include "log/Log2.h"
#include "mpk/rest/ApiGateway.h"
#include "types/strong_typedef/json/JsonParser.hpp"

#include "gsl/pointers"

#include <algorithm>
#include <vector>

namespace mpk::dss::core
{

using JsonParams = rest::ApiGateway::JsonParams;
using JsonResult = rest::ApiGateway::JsonResult;

constexpr auto idTag = "id";
constexpr auto jammersTag = "jammers";
constexpr auto jammerInfoTag = "jammer_info";
constexpr auto jammerGroupsTag = "jammer_groups";
constexpr auto errorStringTag = "error_string";

inline void throwHttpError(http::HttpCode code, const std::string& str)
{
    BOOST_THROW_EXCEPTION(
        exception::http::Exception() << exception::http::ResponseStatus(code)
                                     << exception::http::ResponseMessage(str));
};

inline void throwHttpErrorJson(
    http::HttpCode code, const JsonResult& json_result)
{
    throwHttpError(
        code,
        QJsonDocument(json_result).toJson(QJsonDocument::Compact).toStdString());
}

JammerApiAdapter::JammerApiAdapter(
    rest::ApiGateway& apiGateway,
    std::shared_ptr<JammerProviderPtrs> providers,
    gsl::shared_ptr<JammerGroupProvider> groupProvider,
    std::shared_ptr<WsPublisher> wsPublisher) :
  m_providers(std::move(providers)),
  m_groupProvider(std::move(groupProvider)),
  m_wsPublisher(std::move(wsPublisher))
{
    registerHandlers(apiGateway);
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void JammerApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    apiGateway.registerSyncHandler(
        "jammer/get_jammers",
        http::HttpMethod::GET,
        [this](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO: Thread issues currently omitted
            auto info = std::vector<JammerInfo>{};
            for (const auto& provider: *m_providers)
            {
                std::transform(
                    provider->begin(),
                    provider->end(),
                    std::back_inserter(info),
                    [](const auto& jammer)
                    { return JammerInfo::fromDevice(jammer); });
            }
            return {{jammersTag, json::toValue(info)}};
        });

    apiGateway.registerSyncHandler(
        "jammer/get_jammer_info",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            // TODO: Thread issues currently omitted
            auto id = json::fromObject<JammerDevice::Id>(params, idTag);
            auto* jammer = findDevice(id);
            if (jammer)
            {
                return {
                    {jammerInfoTag,
                     json::toValue(JammerInfo::fromDevice(*jammer))}};
            }

            return {};
        });

    apiGateway.registerSyncHandler(
        "jammer/get_groups",
        http::HttpMethod::GET,
        [this](const JsonParams& /*params*/) -> JsonResult
        {
            // TODO: Thread issues currently omitted
            auto groups = std::vector<QJsonObject>{};
            std::transform(
                m_groupProvider->begin(),
                m_groupProvider->end(),
                std::back_inserter(groups),
                [](const auto& group) { return group.asJsonObject(); });
            return {{jammerGroupsTag, json::toValue(groups)}};
        });

    apiGateway.registerAsyncHandler(
        "jammer/set_jammer_bands",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            LOG_TRACE << "set_jammer_bands params: " << params;

            OAService::OAServiceSet_jammer_bands_request request;
            request.fromJsonObject(params);
            WsReply wsReply(
                "set_jammer_bands",
                request.getWsReplyId(),
                m_wsPublisher.get(),
                true);
            if (!request.isValid())
            {
                QString msg = "POST request set_jammer_bands is not valid";
                LOG_ERROR << "JammerApiAdapter: " << msg;
                wsReply.finish(http::HttpCode::BadRequest, msg);
                return {};
            }
            try
            {
                auto id = static_cast<JammerDevice::Id>(request.getId());
                JammerDevice* jammerDev = findDevice(id);

                auto bandsActive = JammerBandOption(
                    request.getBandsActive().constBegin(),
                    request.getBandsActive().constEnd());
                int duration = request.getDuration();
                auto& timeoutControl = jammerDev->properties().timeoutControl();
                if (jammerDev->properties().disabled())
                {
                    QString msg = "Failed: Jammer is disabled";
                    LOG_ERROR << "JammerApiAdapter: " << msg;
                    wsReply.finish(http::HttpCode::UnprocessableEntity, msg);
                    return {};
                }

                QString message;
                bool success = timeoutControl.jammerStartStop(
                    bandsActive, duration, message);
                wsReply.finish(
                    success ? http::HttpCode::Ok :
                              http::HttpCode::UnprocessableEntity,
                    message);

                return {};
            }
            catch (const exception::http::Exception& ex)
            {
                LOG_ERROR << "JammerApiAdapter: " << ex.what();
                wsReply.finish(ex);
                return {};
            }
            catch (std::exception const& ex)
            {
                LOG_ERROR << "JammerApiAdapter: " << ex.what();
                wsReply.finish(http::HttpCode::InternalServerError, ex.what());
                return {};
            }
        });
}

JammerDevice* JammerApiAdapter::findDevice(const DeviceId& deviceId) const
{
    for (const auto& provider: *m_providers)
    {
        auto it = std::find_if(
            provider->begin(),
            provider->end(),
            [deviceId](const auto& device) { return device.id() == deviceId; });
        if (it != provider->end())
        {
            return &(*it);
        }
    }
    LOG_WARNING << "Jammer not found, id: " << deviceId;
    throwHttpError(http::HttpCode::BadRequest, "Jammer not found");
    return nullptr;
}

} // namespace mpk::dss::core
