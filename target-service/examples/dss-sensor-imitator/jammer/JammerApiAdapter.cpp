#include "JammerApiAdapter.h"

#include "dss-core/json/JammerInfoVsJson.h"

#include "mpk/rest/ApiGateway.h"

#include "types/strong_typedef/json/JsonParser.hpp"

#include <algorithm>
#include <vector>

namespace mpk::dss
{

// constexpr auto jammerTag = "jammer";
// constexpr auto jammerIdTag = "jammer_id";
//[[maybe_unused]] constexpr auto jammerModeTag = "jammer_mode";
// constexpr auto jammerStateTag = "jammer_state";
//[[maybe_unused]] constexpr auto jammerTimeoutTag = "jammer_timeout";

using namespace mpk::dss::core;

namespace
{

using JammerId = DeviceId;

} // namespace

JammerApiAdapter::JammerApiAdapter(
    rest::ApiGateway& apiGateway, std::vector<core::JammerDevice> jammers) :
  m_jammers{std::move(jammers)}
{
    registerHandlers(apiGateway);
}

void JammerApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;
    using StringResult = rest::ApiGateway::StringResult;

    apiGateway.registerSyncHandler(
        "get_jammers",
        [this](const JsonParams& /*params*/) -> StringResult
        {
            QJsonArray result;
            std::transform(
                m_jammers.begin(),
                m_jammers.end(),
                std::back_inserter(result),
                [](const auto& jammer)
                { return json::toValue(JammerInfo::fromDevice(jammer)); });
            return QJsonDocument(result).toJson(QJsonDocument::Compact).toStdString();
        });

    apiGateway.registerSyncHandler(
        "get_jammer",
        [/*this*/](const JsonParams& /*params*/) -> JsonResult
        {
            /*const auto id = json::fromObject<JammerDevice::Id>(params, jammerIdTag);
            auto it = std::find_if(
                m_jammers.begin(),
                m_jammers.end(),
                [id](const auto& jammerDevice)
                { return jammerDevice->id() == id; });
            if (it != m_jammers->end())
            {
                return {
                    {jammerInfoTag,
                     json::toValue(JammerInfo::fromDevice(**it))}};
            }

            auto jammer = findById(m_jammers, id);
            if (jammer != m_jammers.end())
            {
                return QJsonObject{{jammerTag, json::toValue(*jammer)}};
            }*/
            return {};
        });

    apiGateway.registerSyncHandler(
        "get_jammer_mode",
        http::HttpMethod::GET,
        [](const QJsonObject&) -> QJsonObject
        {
            // TODO:
            return {};
        });

    apiGateway.registerAsyncHandler(
        "set_jammer_mode",
        [](const QJsonObject&)
        {
            // TODO:
        });

    apiGateway.registerSyncHandler(
        "get_jammer_state",
        http::HttpMethod::GET,
        [/*this*/](const QJsonObject& /*json*/) -> QJsonObject
        {
            /*auto id = json::fromObject<DeviceId>(json, jammerIdTag);
            auto jammer = findById(m_jammers, id);
            if (jammer != m_jammers.end())
            {
                return {
                    {jammerStateTag,
                     DeviceState::toString((*jammer)->state())}};
            }*/
            return {};
        });

    apiGateway.registerAsyncHandler(
        "set_jammer_state", [](const QJsonObject& /*unused*/) {});

    apiGateway.registerSyncHandler(
        "get_jammer_timeout",
        http::HttpMethod::GET,
        [](const QJsonObject& /*unused*/) -> QJsonObject { return {}; });

    apiGateway.registerAsyncHandler(
        "set_jammer_timeout", [](const QJsonObject& /*unused*/) {});
}

} // namespace mpk::dss
