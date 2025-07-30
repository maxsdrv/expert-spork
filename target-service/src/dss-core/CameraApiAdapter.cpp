#include "CameraApiAdapter.h"

#include "DeviceId.h"
#include "TrackId.h"

#include "OAServiceCamera_id.h"

#include "json/JsonParser.h"
#include "log/Log2.h"
#include "mpk/rest/ApiGateway.h"
#include "types/strong_typedef/json/JsonParser.hpp"

#include <QJsonObject>

namespace mpk::dss::core
{

constexpr auto idTag = "id";

CameraApiAdapter::CameraApiAdapter(
    rest::ApiGateway& apiGateway,
    std::shared_ptr<CameraProviderPtrs> providers) :
  m_providers(std::move(providers))
{
    registerHandlers(apiGateway);
}

void CameraApiAdapter::registerHandlers(rest::ApiGateway& apiGateway)
{
    using JsonParams = rest::ApiGateway::JsonParams;
    using JsonResult = rest::ApiGateway::JsonResult;

    apiGateway.registerSyncHandler(
        "camera/get_id",
        http::HttpMethod::GET,
        [this](const JsonParams& params) -> JsonResult
        {
            auto id = json::fromObject<DeviceId>(params, idTag);
            auto* camera = findCamera(id);
            if (nullptr == camera)
            {
                // TODO: return http error
                return {};
            }

            auto result = OAService::OAServiceCamera_id();
            result.setCameraId(camera->properties().id());
            return result.asJsonObject();
        });
}

CameraDevice* CameraApiAdapter::findCamera(const DeviceId& id) const
{
    for (const auto& provider: *m_providers)
    {
        auto it = std::find_if(
            provider->begin(),
            provider->end(),
            [id](const auto& device) { return device.id() == id; });
        if (it != provider->end())
        {
            return &(*it);
        }
    }
    LOG_WARNING << "Camera not found, id: " << id;
    return nullptr;
};

} // namespace mpk::dss::core
