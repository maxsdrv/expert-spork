#include "SensorApiAdapter.h"
#include "JammerTimeoutControl.h"
#include "SensorDevice.h"
#include "SensorProperties.h"
#include "database/JammerTimeoutStatusDbHelper.h"

#include "OAServiceSet_jammer_mode_request.h"

#include "httpws/HttpException.h"
#include "httpws/HttpMethod.h"
#include "json/FromJson.h"
#include "json/SensorVsJson.h"

#include "log/Log2.h"
#include "types/strong_typedef/json/JsonParser.hpp"

#include "mpk/rest/ApiGateway.h"

namespace mpk::dss::core
{

constexpr auto idTag = "id";
constexpr auto sensorsTag = "sensors";
constexpr auto sensorInfoTag = "sensor_info";
constexpr auto jammerModeTag = "jammer_mode";

inline void throwHttpError(http::HttpCode code, const std::string& str)
{
    BOOST_THROW_EXCEPTION(
        exception::http::Exception() << exception::http::ResponseStatus(code)
                                     << exception::http::ResponseMessage(str));
};

SensorApiAdapter::SensorApiAdapter(
    rest::ApiGateway& apiGateway,
    std::shared_ptr<SensorProviderPtrs> providers) :
  m_providers(std::move(providers))
{
    registerHandlers(apiGateway);
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void SensorApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerSyncHandler(
        "sensor/get_sensors",
        http::HttpMethod::GET,
        [this]() -> JsonResult
        {
            std::vector<QJsonObject> sensorsJson;
            for (const auto& provider: *m_providers)
            {
                std::transform(
                    provider->begin(),
                    provider->end(),
                    std::back_inserter(sensorsJson),
                    [](const auto& sensor)
                    { return json::toValue(sensor).toObject(); });
            }
            return {{sensorsTag, json::toValue(sensorsJson)}};
        });

    apiGateway.registerSyncHandler(
        "sensor/get_sensor_info",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id = json::fromObject<SensorDevice::Id>(params, idTag);
            LOG_DEBUG << "GET get_sensor_info request id: " << id;
            auto* sensor = findDevice(id);
            if (sensor)
            {
                return QJsonObject{{sensorInfoTag, json::toValue(*sensor)}};
            }

            return {};
        });

    apiGateway.registerSyncHandler(
        "sensor/get_jammer_mode",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id = json::fromObject<SensorDevice::Id>(params, idTag);
            auto* sensor = findDevice(id);
            if (sensor)
            {
                return QJsonObject{
                    {jammerModeTag,
                     json::toValue(JammerMode::toString(
                         sensor->properties().jammerMode()))}};
            }

            return {};
        });

    apiGateway.registerAsyncHandler(
        "sensor/set_jammer_mode",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceSet_jammer_mode_request request;
            request.fromJsonObject(params);
            if (!request.isValid())
            {
                LOG_ERROR << "POST request set_jammer_mode is not valid";
                return {};
            }
            auto mode =
                JammerMode::fromString(request.getJammerMode().asJson());
            if (request.getTimeout() <= 0 && mode == JammerMode::AUTO)
            {
                LOG_ERROR
                    << "POST request set_jammer_mode timeout value is "
                    << request.getTimeout()
                    << " but expected to be greater than zero in AUTO mode";
                return {};
            }
            auto id = static_cast<SensorDevice::Id>(request.getId());
            auto* sensor = findDevice(id);
            if (sensor)
            {
                sensor->properties().setJammerMode(
                    mode, seconds(request.getTimeout()));

                JammerTimeoutStatusDbHelper jammerDb;
                jammerDb.saveMode(
                    static_cast<QString>(id),
                    mode,
                    seconds(request.getTimeout()));
            }

            return {};
        });
}

SensorDevice* SensorApiAdapter::findDevice(const DeviceId& deviceId) const
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
    LOG_WARNING << "Sensor not found, id: " << deviceId;
    throwHttpError(::http::HttpCode::BadRequest, "Sensor not found");
    return nullptr;
}

} // namespace mpk::dss::core
