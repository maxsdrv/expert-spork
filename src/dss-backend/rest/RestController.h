#pragma once

#include "dss-backend/BackendControllerBase.h"
#include "dss-backend/rest/SensorTargetData.h"
#include "dss-core/TrackId.h"

#include "mpk/rest/HttpWsConnection.h"

#include <yaml-cpp/yaml.h>

#include <memory>
#include <vector>

class QTimer;

namespace mpk::dss::backend::rest
{

constexpr auto nodeKey = "rest";

class RestController : public BackendControllerBase
{

public:
    static BackendControllerPtr fromDescription(
        const YAML::Node& description, const BackendComponents& components);

private:
    explicit RestController(const BackendComponents& components);

private:
    void handleWsMessage(
        const core::DeviceId& deviceId, const QJsonObject& json);
    void removeTrack(const core::TrackId& trackId);

private:
    using OriginTrackId = decltype(SensorTargetData::id);
    struct TrackData
    {
        core::DeviceId deviceId;
        core::TrackId trackId;
        OriginTrackId originId = 0;
        std::unique_ptr<QTimer> timer;
    };

    std::vector<TrackData> m_trackData;
};

} // namespace mpk::dss::backend::rest
