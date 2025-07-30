#include "QtGeoJson.h"

#include <cmath>

namespace
{

const auto altitudeTag = "altitude";
const auto latitudeTag = "latitude";
const auto longitudeTag = "longitude";

} // namespace

QJsonValue ToJsonConverter<QGeoCoordinate>::convert(const QGeoCoordinate& coord)
{
    auto object = QJsonObject{
        {latitudeTag, json::toValue(coord.latitude())},
        {longitudeTag, json::toValue(coord.longitude())}};

    if (!std::isnan(coord.altitude()))
    {
        object.insert(altitudeTag, json::toValue(coord.altitude()));
    }
    return object;
}

QGeoCoordinate FromJsonConverter<QGeoCoordinate>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        json::fromObject<double>(object, latitudeTag),
        json::fromObject<double>(object, longitudeTag),
        json::fromObjectWithDefault<double>(object, altitudeTag, NAN)};
}

QJsonValue ToJsonConverter<QGeoPolygon>::convert(const QGeoPolygon& polygon)
{
    return json::toValue(polygon.path());
}

QGeoPolygon FromJsonConverter<QGeoPolygon>::get(const QJsonValue& value)
{
    return json::fromValue<QList<QGeoCoordinate>>(value);
}
