#pragma once

#include "AlarmStatus.h"
#include "TargetClass.h"
#include "Track.h"

#include "OAServiceCamera_track.h"

#include "boost/iterators/ForwardIterator.h"

#include "gsl/pointers"

#include <QObject>

#include <vector>

namespace mpk::dss::core
{

class TrackController : public QObject
{
    Q_OBJECT

public:
    explicit TrackController(QObject* parent = nullptr);

    using Iterator = mpkit::ForwardIterator<Track const>;

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;
    [[nodiscard]] Iterator find(const TrackId& trackId) const;

public slots:
    void handleTrackStart(
        const DeviceId& deviceId,
        const TrackId& trackId,
        const OAService::OAServiceCamera_track& cameraTrack,
        const AlarmStatus& alarmStatus,
        const TargetClass::Value& className,
        const QJsonObject& attributes);
    void handleTrackUpdate(
        const DeviceId& deviceId,
        const TrackId& trackId,
        const AlarmStatus& alarmStatus,
        const TargetClass::Value& className,
        const QJsonObject& attributes,
        const TrackPoints& trackPoints);
    void handleTrackFinish(
        const core::DeviceId& deviceId, const core::TrackId& trackId);

signals:
    void started(
        DeviceId,
        TrackId,
        OAService::OAServiceCamera_track,
        AlarmStatus,
        TargetClass::Value,
        QJsonObject attributes);
    void updated(
        TrackId, AlarmStatus, TargetClass::Value, QJsonObject attributes);
    void finished(TrackId);

private:
    std::vector<Track> m_tracks;
};

} // namespace mpk::dss::core
