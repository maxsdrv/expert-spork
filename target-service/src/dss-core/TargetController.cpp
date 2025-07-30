#include "TargetController.h"

#include "TrackController.h"

#include "log/Log2.h"

#include "boost/iterators/details/ValueTypeForwardIterator.h"

#include <QUuid>

namespace mpk::dss::core
{

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
TargetController::TargetController(
    gsl::not_null<const TrackController*> trackController,
    AlarmLevel::Value minPriorityThreshold,
    QObject* parent) :
  QObject(parent),
  m_trackController(trackController),
  m_minPriorityThreshold(minPriorityThreshold)
{
    connect(
        m_trackController,
        &mpk::dss::core::TrackController::started,
        this,
        [this](
            const auto& sensorId,
            const auto& trackId,
            const auto& cameraTrack,
            const auto& alarmStatus,
            const auto& targetClassName,
            const auto& targetAttributes)
        {
            auto it = std::find_if(
                m_targets.begin(),
                m_targets.end(),
                [trackId](const auto& targetData)
                { return targetData.trackId == trackId; });
            if (it == m_targets.end())
            {
                auto id = TargetId::generate();
                auto target = TargetData{
                    .id = id,
                    .sensorId = sensorId,
                    .trackId = trackId,
                    .cameraTrack = {},
                    .alarmIds = {},
                    .clsIds = {},
                    .className = targetClassName,
                    .attributes = targetAttributes,
                    .alarmStatus = alarmStatus,
                    .position = {},
                    .lastUpdated = QDateTime::currentDateTimeUtc(),
                    .rawData = {}};
                if (cameraTrack.isSet())
                {
                    target.cameraTrack = cameraTrack;
                }
                m_targets.push_back(target);

                LOG_DEBUG << "Target controller: target " << id << " detected";

                if (matchPriorityThreshold(alarmStatus.level))
                {
                    LOG_INFO << "Target controller: target " << id
                             << "notified";
                    emit detected(id);
                }
                else
                {
                    LOG_DEBUG << "Target controller: target " << id
                              << "filtered";
                }
            }
            else
            {
                LOG_WARNING << "Target controller: Duplicate track id: "
                            << trackId;
            }
        });

    connect(
        m_trackController,
        &TrackController::updated,
        this,
        [this](
            const auto& trackId,
            const auto& alarmStatus,
            const auto& targetClass,
            const auto& targetAttributes,
            const auto& detectCount)
        {
            auto trackIt = m_trackController->find(trackId);
            Expects(trackIt != m_trackController->end());

            auto it = std::find_if(
                m_targets.begin(),
                m_targets.end(),
                [trackId](const auto& targetData)
                { return targetData.trackId == trackId; });
            if (it != m_targets.end())
            {
                if (!trackIt->points.empty())
                {
                    const auto& lastPoint = trackIt->points.back();
                    it->position.coordinate = lastPoint.coordinate;
                    it->position.direction = lastPoint.direction;
                    it->lastUpdated = lastPoint.timestamp;
                    it->className = targetClass;
                    it->attributes = targetAttributes;
                    it->alarmStatus = alarmStatus;
                    it->detectCount = detectCount;
                }

                LOG_DEBUG << "Target controller: target " << it->id
                          << " updated";

                if (matchPriorityThreshold(alarmStatus.level))
                {
                    LOG_INFO << "Target controller: target " << it->id
                             << "notified";
                    emit updated(it->id);
                }
                else
                {
                    LOG_DEBUG << "Target controller: target " << it->id
                              << "filtered";
                }
            }
            else
            {
                LOG_WARNING << "Target controller: Missing track id: "
                            << trackId;
            }
        });

    connect(
        m_trackController,
        &mpk::dss::core::TrackController::finished,
        this,
        [this](const auto& trackId)
        {
            auto it = std::find_if(
                m_targets.begin(),
                m_targets.end(),
                [trackId](const auto& targetData)
                { return targetData.trackId == trackId; });
            if (it != m_targets.end())
            {
                auto id = it->id;
                auto targetPriority = it->alarmStatus.level;
                m_targets.erase(it);

                LOG_DEBUG << "Target controller: target " << id << " lost";

                if (matchPriorityThreshold(targetPriority))
                {
                    LOG_INFO << "Target controller: target " << id
                             << "notified";
                    emit lost(id);
                }
                else
                {
                    LOG_DEBUG << "Target controller: target " << id
                              << "filtered";
                }
            }
            else
            {
                LOG_WARNING << "Target controller: Missing track id: "
                            << trackId;
            }
        });
}

using PrivateIterator = mpkit::details::ValueTypeForwardIterator<
    std::vector<TargetData>::const_iterator,
    TargetData const>;

auto TargetController::begin() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_targets.begin())};
}

auto TargetController::end() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_targets.end())};
}

auto TargetController::find(const TargetId& targetId) const -> Iterator
{
    auto it = std::find_if(
        m_targets.begin(),
        m_targets.end(),
        [targetId](const auto& target) { return target.id == targetId; });
    return Iterator{std::make_shared<PrivateIterator>(it)};
}

bool TargetController::matchPriorityThreshold(
    AlarmLevel::Value currentLevel) const
{
    if (currentLevel == AlarmLevel::UNDEFINED)
    {
        return false;
    }

    if (m_minPriorityThreshold == AlarmLevel::UNDEFINED)
    {
        return false;
    }

    return static_cast<int>(currentLevel)
           >= static_cast<int>(m_minPriorityThreshold);
}

} // namespace mpk::dss::core
