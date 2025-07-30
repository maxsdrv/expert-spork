#include "TrackVsJson.h"

#include "QtGeoJson.h"
#include "TargetDirectionVsJson.h"

#include "types/strong_typedef/json/JsonComposer.hpp"

namespace
{

constexpr auto targetIdTag = "target_id";
constexpr auto trackIdTag = "track_id";
constexpr auto sourceIdTag = "source_id";
constexpr auto pointsTag = "points";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<Track>::convert(const Track& track)
{
    return QJsonObject{
        {trackIdTag, json::toValue(track.id)},
        {targetIdTag, json::toValue(track.targetId)},
        {sourceIdTag, json::toValue(track.sourceId)},
        {pointsTag, json::toValue(track.points)}};
}

QJsonValue ToJsonConverter<TrackPoint>::convert(const TrackPoint& trackPoint)
{
    return QJsonObject{
        {trackIdTag, json::toValue(trackPoint.coordinate)},
        {targetIdTag, json::toValue(trackPoint.direction)},
        {sourceIdTag, json::toValue(trackPoint.timestamp)},
        {pointsTag, json::toValue(trackPoint.rawData)}};
}
