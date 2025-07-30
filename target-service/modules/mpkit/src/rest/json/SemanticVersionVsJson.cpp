#include "mpk/rest/json/SemanticVersionVsJson.h"

#include "mpk/rest/tag/SemanticVersionTag.h"

using SemanticVersion = mpk::rest::SemanticVersion;
using Tag = mpk::rest::SemanticVersionTag;

QJsonValue ToJsonConverter<SemanticVersion>::convert(const SemanticVersion& value)
{
    return QJsonObject{
        {Tag::major, json::toValue(value.major)},
        {Tag::minor, json::toValue(value.minor)},
        {Tag::patch, json::toValue(value.patch)},
    };
}

SemanticVersion FromJsonConverter<SemanticVersion>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);

    return SemanticVersion{
        json::fromObjectWithDefault(object, Tag::major, 0U),
        json::fromObjectWithDefault(object, Tag::minor, 0U),
        json::fromObjectWithDefault(object, Tag::patch, 0U)};
}
