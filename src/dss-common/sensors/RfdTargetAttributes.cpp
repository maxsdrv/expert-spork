#include "RfdTargetAttributes.h"

namespace
{

constexpr auto digitalTag = "digital";
constexpr auto frequenciesTag = "frequencies";
constexpr auto descriptionTag = "description";
constexpr auto geolocationTag = "geolocation";

} // namespace

using namespace mpk::dss::sensors;

QJsonValue ToJsonConverter<RfdTargetAttributes>::convert(
    const RfdTargetAttributes& data)
{
    return QJsonObject{
        {digitalTag, json::toValue(data.digital)},
        {frequenciesTag, json::toValue(data.frequencies)},
        {descriptionTag, json::toValue(data.description)},
        {geolocationTag, json::toValue(data.geolocation)}};
}
