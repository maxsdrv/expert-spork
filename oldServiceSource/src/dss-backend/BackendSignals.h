#pragma once

#include "dss-core/AlarmStatus.h"
#include "dss-core/DeviceId.h"
#include "dss-core/TargetClass.h"
#include "dss-core/Track.h"

#include "OAServiceCamera_track.h"

namespace mpk::dss::backend
{

class BackendSignals : public QObject
{
    Q_OBJECT

public:
    explicit BackendSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void trackStarted(
        core::DeviceId,
        core::TrackId,
        OAService::OAServiceCamera_track,
        core::AlarmStatus,
        core::TargetClass::Value,
        QJsonObject attributes);
    void trackUpdated(
        core::DeviceId,
        core::TrackId,
        core::AlarmStatus,
        core::TargetClass::Value,
        QJsonObject attributes,
        core::TrackPoints);
    void trackFinished(core::DeviceId, core::TrackId);
};

} // namespace mpk::dss::backend
