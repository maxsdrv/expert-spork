#pragma once

#include "dss-core/AlarmId.h"
#include "dss-core/AlarmStatus.h"
#include "dss-core/ClsId.h"
#include "dss-core/DeviceId.h"
#include "dss-core/TargetClass.h"
#include "dss-core/TargetId.h"
#include "dss-core/TargetPosition.h"
#include "dss-core/TrackId.h"

#include "OAServiceCamera_track.h"

#include <QDateTime>

namespace mpk::dss::core
{

struct TargetData
{
    TargetId id;
    DeviceId sensorId;
    TrackId trackId;
    std::optional<OAService::OAServiceCamera_track> cameraTrack;
    AlarmIds alarmIds;
    ClsIds clsIds;
    TargetClass::Value className;
    QJsonObject attributes;
    AlarmStatus alarmStatus;
    TargetPosition position;
    QDateTime lastUpdated;
    QJsonObject rawData;
};

} // namespace mpk::dss::core
