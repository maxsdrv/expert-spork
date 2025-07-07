#include "GeolocationVsJson.h"

#include "dss-core/json/QtGeoJson.h"

namespace
{

constexpr auto coordinateTag = "coordinate";
constexpr auto azimuthTag = "azimuth";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<Geolocation>::convert(const Geolocation& data)
{
    return QJsonObject{
        {coordinateTag, json::toValue(data.location)},
        {azimuthTag, json::toValue(data.azimuth)}};
}

Geolocation FromJsonConverter<Geolocation>::get(const QJsonValue& value)
{
    const json::ObjectParser json{value};
    Geolocation geolocation;
    json.to(geolocation.location, coordinateTag);
    json.to(geolocation.azimuth, azimuthTag);

    return geolocation;
}
