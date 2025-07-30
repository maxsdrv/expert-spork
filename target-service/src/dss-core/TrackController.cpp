#include "TrackController.h"

#include "boost/iterators/details/ValueTypeForwardIterator.h"

namespace mpk::dss::core
{

TrackController::TrackController(QObject* parent) : QObject(parent)
{
}

void TrackController::handleTrackStart(
    const DeviceId& deviceId,
    const TrackId& trackId,
    const OAService::OAServiceCamera_track& cameraTrack,
    const AlarmStatus& alarmStatus,
    const TargetClass::Value& className,
    const QJsonObject& attributes)
{
    auto it = std::find_if(
        m_tracks.begin(),
        m_tracks.end(),
        [deviceId, trackId](const auto& track) { return track.id == trackId; });
    if (it == m_tracks.end())
    {
        auto track = Track{
            .id = trackId, .targetId = {}, .sourceId = deviceId, .points = {}};

        m_tracks.push_back(track);
        emit started(
            deviceId, trackId, cameraTrack, alarmStatus, className, attributes);
    }
}

void TrackController::handleTrackUpdate(
    const DeviceId& deviceId,
    const TrackId& trackId,
    const AlarmStatus& alarmStatus,
    const TargetClass::Value& className,
    const QJsonObject& attributes,
    const TrackPoints& trackPoints,
    const qint64& detectCount)
{
    auto it = std::find_if(
        m_tracks.begin(),
        m_tracks.end(),
        [deviceId, trackId](const auto& track) { return track.id == trackId; });
    if (it != m_tracks.end())
    {
        it->points.insert(
            it->points.end(), trackPoints.begin(), trackPoints.end());

        emit updated(it->id, alarmStatus, className, attributes, detectCount);
    }
}

void TrackController::handleTrackFinish(
    const core::DeviceId& deviceId, const core::TrackId& trackId)
{
    auto it = std::find_if(
        m_tracks.begin(),
        m_tracks.end(),
        [deviceId, trackId](const auto& track) { return track.id == trackId; });
    if (it != m_tracks.end())
    {
        auto id = it->id;

        m_tracks.erase(it);
        emit finished(id);
    }
}

using PrivateIterator = mpkit::details::
    ValueTypeForwardIterator<std::vector<Track>::const_iterator, Track const>;

auto TrackController::begin() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_tracks.begin())};
}

auto TrackController::end() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_tracks.end())};
}

auto TrackController::find(const TrackId& trackId) const -> Iterator
{
    auto it = std::find_if(
        m_tracks.begin(),
        m_tracks.end(),
        [trackId](const auto& track) { return track.id == trackId; });
    return Iterator{std::make_shared<PrivateIterator>(it)};
}

} // namespace mpk::dss::core
