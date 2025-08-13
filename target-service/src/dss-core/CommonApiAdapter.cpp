#include "CommonApiAdapter.h"

#include "dss-core/DeviceApiAdapter.h"
#include "dss-core/DeviceIdMapping.h"
#include "dss-core/DevicePositionStorage.h"
#include "dss-core/database/DatabaseError.h"
#include "dss-core/database/DbServiceFactory.h"
#include "dss-core/database/DeviceDisabledDbHelper.h"
#include "dss-core/license/LicenseController.h"

#include "OAServiceSettings.h"
#include "OAServiceUpload_license_request.h"

#include "mpk/rest/ApiGateway.h"

#include "httpws/HttpException.h"
#include "httpws/HttpMethod.h"
#include "json/JsonParser.h"

#include <QJsonObject>

namespace mpk::dss::core
{

constexpr auto apiVersionTag = "api_version";
constexpr auto apiVersion = "2.1.0";

inline void throwHttpError(http::HttpCode code, const std::string& str)
{
    BOOST_THROW_EXCEPTION(
        exception::http::Exception() << exception::http::ResponseStatus(code)
                                     << exception::http::ResponseMessage(str));
};

CommonApiAdapter::CommonApiAdapter(
    rest::ApiGateway& apiGateway,
    gsl::not_null<LicenseController*> licenseController,
    std::shared_ptr<SensorProviderPtrs> sensorProviders,
    std::shared_ptr<JammerProviderPtrs> jammerProviders) :
  m_licenseController(licenseController),
  m_sensorProviders(std::move(sensorProviders)),
  m_jammerProviders(std::move(jammerProviders))
{
    registerHandlers(apiGateway);
}

void CommonApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerSyncHandler(
        "common/get_api_version",
        []() {
            return QJsonObject{{apiVersionTag, apiVersion}};
        });

    apiGateway.registerSyncHandler(
        "common/get_license_status",
        [this]() { return m_licenseController->status().asJsonObject(); });

    apiGateway.registerSyncHandler(
        "common/upload_license",
        http::HttpMethod::POST,
        [this](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceUpload_license_request request;
            request.fromJsonObject(params);
            if (!request.isValid())
            {
                throwHttpError(
                    ::http::HttpCode::BadRequest, "Request is not valid");
            }
            auto result =
                m_licenseController->assign(request.getLicenseData().toUtf8());
            if (!result)
            {
                throwHttpError(
                    ::http::HttpCode::UnprocessableEntity,
                    "License data is not valid");
            }
            return {};
        });
    apiGateway.registerSyncHandler(
        "common/get_settings",
        http::HttpMethod::GET,
        [this](const JsonParams&) -> JsonResult
        {
            OAService::OAServiceSettings response;
            auto oaDevices =
                getDevicesSettings(m_sensorProviders, m_jammerProviders);
            response.setDevices(oaDevices);

            DeviceIdMappingStorage deviceIdMappingStorage(createDbService());

            QList<OAService::OAServiceDevice_id_mapping> oaDevIdMappingList;
            std::vector<DeviceIdMapping> vecDevMapping =
                deviceIdMappingStorage.fetchData();

            for (const auto& dbMapping: vecDevMapping)
            {
                OAService::OAServiceDevice_id_mapping oaMapping;
                oaMapping.setId(static_cast<QString>(dbMapping.id));
                auto key = DeviceIdMappingKey::fromString(dbMapping.key);
                oaMapping.setFingerprint(key.fingerprint);
                oaMapping.setRClass(DeviceClass::toString(key.type));
                oaDevIdMappingList.append(oaMapping);
            }
            response.setDeviceIdMapping(oaDevIdMappingList);
            return response.asJsonObject();
        });
    apiGateway.registerAsyncHandler(
        "common/set_settings",
        http::HttpMethod::POST,
        [](const JsonParams& params) -> JsonResult
        {
            OAService::OAServiceSettings request;
            request.fromJsonObject(params);
            if (!request.isValid())
            {
                LOG_ERROR << "POST request common/set_settings is not valid";
                return {};
            }

            try
            {
                // Clear and fill order is important
                auto dbService = createDbService();
                DevicePositionStorage(dbService).clear();
                DeviceDisabledDbHelper dbDisabled;
                dbDisabled.clear();
                DeviceIdMappingStorage deviceIdMappingStorage(dbService);
                deviceIdMappingStorage.clear();

                QList<OAService::OAServiceDevice_id_mapping> oaMappingList =
                    request.getDeviceIdMapping();
                for (const auto& oaMapping: oaMappingList)
                {
                    deviceIdMappingStorage.modifier()->append(DeviceIdMapping{
                        .key = DeviceIdMappingKey::toString(DeviceIdMappingKey{
                            oaMapping.getFingerprint(),
                            DeviceClass::fromString(oaMapping.getRClass())}),
                        .id = DeviceId(oaMapping.getId())});
                }

                const auto devices = request.getDevices();
                for (const auto& oaSettings: devices)
                {
                    DevicePositionStorage devicePositionStorage(
                        createDbService());
                    auto pos = oaSettings.getPosition();
                    DevicePositionExtended position = {
                        .key = oaSettings.getId(),
                        .mode = oaSettings.getPositionMode().getValue(),
                        .position = {
                            pos.getAzimuth(),
                            {pos.getCoordinate().getLatitude(),
                             pos.getCoordinate().getLongitude(),
                             pos.getCoordinate().getAltitude()}}};
                    devicePositionStorage.modifier()->append(position);
                }

                dbDisabled.saveDisabled(request.getDevices());
            }
            catch (exception::DatabaseError& ex)
            {
                LOG_ERROR
                    << "POST request common/set_settings: database error: "
                    << ex.what();
                return {};
            }

            constexpr int RestartExitCode = 99;
            // Will be restarted by external script
            std::exit(RestartExitCode);

            return {};
        });
}

} // namespace mpk::dss::core
