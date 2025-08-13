
#include "DeviceApiAdapter.h"
#include "database/DatabaseError.h"
#include "database/DbServiceFactory.h"
#include "database/DeviceDisabledDbHelper.h"

#include "OAServiceSet_disabled_request.h"
#include "OAServiceSet_position_mode_request.h"
#include "OAServiceSet_position_request.h"

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
constexpr auto positionModeTag = "position_mode";

inline void throwHttpError(http::HttpCode code, const std::string& str)
{
    BOOST_THROW_EXCEPTION(
        exception::http::Exception() << exception::http::ResponseStatus(code)
                                     << exception::http::ResponseMessage(str));
};

template <typename Interface>
Interface* findDevice(
    const DeviceId& deviceId,
    const SensorProviderPtrs* sensorProviders,
    const JammerProviderPtrs* jammerProviders)
{
    for (const auto& provider: *sensorProviders)
    {
        auto it = std::find_if(
            provider->begin(),
            provider->end(),
            [deviceId](const auto& device) { return device.id() == deviceId; });
        if (it != provider->end())
        {
            return &(it->properties());
        }
    }
    for (const auto& provider: *jammerProviders)
    {
        auto it = std::find_if(
            provider->begin(),
            provider->end(),
            [deviceId](const auto& device) { return device.id() == deviceId; });
        if (it != provider->end())
        {
            return &(it->properties());
        }
    }
    LOG_WARNING << "Device not found, id: " << deviceId;
    throwHttpError(::http::HttpCode::BadRequest, "Device not found");
    return nullptr;
}

void setDevicePositionDb(
    const QString& id,
    const OAService::OAServiceGeo_position& positionObj,
    const SensorProviderPtrs* sensorProviders,
    const JammerProviderPtrs* jammerProviders)
{
    auto altitude = positionObj.getCoordinate().is_altitude_Set() ?
                        positionObj.getCoordinate().getAltitude() :
                        0.0;
    auto position = DevicePosition{
        .azimuth = positionObj.getAzimuth(),
        .coordinate = QGeoCoordinate(
            positionObj.getCoordinate().getLatitude(),
            positionObj.getCoordinate().getLongitude(),
            altitude)};
    auto* device = findDevice<DevicePositionControl>(
        static_cast<DeviceId>(id), sensorProviders, jammerProviders);

    DevicePositionStorage devicePositionStorage(createDbService());
    auto devicePosition = devicePositionStorage.find(id);
    if (devicePosition)
    {
        if (devicePosition->position != position)
        {
            devicePosition->position = position;
            devicePositionStorage.modifier()->replace(id, *devicePosition);
            device->setPosition(position);
        }
    }
    else
    {
        DevicePositionExtended positionToAppend = {
            .key = id, .mode = device->positionMode(), .position = position};
        devicePositionStorage.modifier()->append(positionToAppend);
        device->setPosition(position);
    }
}

void setDevicePositionModeDb(
    const QString& id,
    const DevicePositionMode& mode,
    const SensorProviderPtrs* sensorProviders,
    const JammerProviderPtrs* jammerProviders)
{
    if (mode == core::DevicePositionMode::ALWAYS_MANUAL)
    {
        throwHttpError(
            ::http::HttpCode::UnprocessableEntity,
            "This position mode cannot be set");
    }
    auto* device = findDevice<DevicePositionControl>(
        static_cast<DeviceId>(id), sensorProviders, jammerProviders);
    if (device)
    {
        DevicePositionStorage devicePositionStorage(createDbService());
        auto devicePosition = devicePositionStorage.find(id);
        if (devicePosition)
        {
            if (devicePosition->mode == core::DevicePositionMode::ALWAYS_MANUAL)
            {
                throwHttpError(
                    ::http::HttpCode::UnprocessableEntity,
                    "Position mode cannot be changed for this device");
            }
            if (devicePosition->mode != mode)
            {
                devicePosition->mode = mode;
                devicePositionStorage.modifier()->replace(id, *devicePosition);
                device->setPositionMode(mode);
            }
        }
        else
        {
            if (device->positionMode()
                == core::DevicePositionMode::ALWAYS_MANUAL)
            {
                throwHttpError(
                    ::http::HttpCode::UnprocessableEntity,
                    "Position mode cannot be changed for this device");
            }
            DevicePositionExtended position = {
                .key = id, .mode = mode, .position = device->position()};
            devicePositionStorage.modifier()->append(position);
        }
    }
}

DeviceApiAdapter::DeviceApiAdapter(
    rest::ApiGateway& apiGateway,
    std::shared_ptr<SensorProviderPtrs> sensorProviders,
    std::shared_ptr<JammerProviderPtrs> jammerProviders) :
  m_sensorProviders(std::move(sensorProviders)),
  m_jammerProviders(std::move(jammerProviders))
{
    registerHandlers(apiGateway);
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void DeviceApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerSyncHandler(
        "device/set_disabled",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceSet_disabled_request request;
            request.fromJsonObject(params);
            if (!request.isValid())
            {
                throwHttpError(
                    ::http::HttpCode::BadRequest,
                    "POST request set_sensor_disabled is not valid");
            }
            auto id = static_cast<DeviceId>(request.getId());
            auto* device = findDevice<DeviceDisabledControl>(
                id, m_sensorProviders.get(), m_jammerProviders.get());
            if (device)
            {
                auto disabled = request.isDisabled();
                device->setDisabled(disabled);
            }
            try
            {
                DeviceDisabledDbHelper().saveDisabled(
                    getDevicesSettings(m_sensorProviders, m_jammerProviders));
            }
            catch (exception::DatabaseError& ex)
            {
                throwHttpError(::http::HttpCode::InternalServerError, ex.what());
            }

            return {};
        });

    apiGateway.registerSyncHandler(
        "device/get_position_mode",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id =
                static_cast<QString>(json::fromObject<DeviceId>(params, idTag));
            auto* device = findDevice<DevicePositionControl>(
                static_cast<DeviceId>(id),
                m_sensorProviders.get(),
                m_jammerProviders.get());

            OAService::OAServiceGeo_position_mode mode;
            mode.setValue(device->positionMode());
            return QJsonObject{{positionModeTag, mode.asJson()}};

            return {};
        });

    apiGateway.registerSyncHandler(
        "device/set_position_mode",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceSet_position_mode_request request;
            request.fromJsonObject(params);
            if (!request.isValid() || !request.getPositionMode().isValid())
            {
                throwHttpError(
                    ::http::HttpCode::BadRequest,
                    "POST request set_position_mode is not valid");
            }

            setDevicePositionModeDb(
                request.getId(),
                request.getPositionMode().getValue(),
                m_sensorProviders.get(),
                m_jammerProviders.get());

            return {};
        });

    apiGateway.registerSyncHandler(
        "device/set_position",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceSet_position_request request;
            request.fromJsonObject(params);
            if (!request.isValid())
            {
                throwHttpError(
                    ::http::HttpCode::BadRequest,
                    "POST request set_position is not valid");
            }

            setDevicePositionDb(
                request.getId(),
                request.getPosition(),
                m_sensorProviders.get(),
                m_jammerProviders.get());

            return {};
        });
}

} // namespace mpk::dss::core
