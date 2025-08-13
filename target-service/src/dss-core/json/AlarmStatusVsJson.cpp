#include "dss-core/json/AlarmStatusVsJson.h"

#include "json/parsers/EnumClass.h"

namespace
{

constexpr auto levelTag = "level";
constexpr auto scoreTag = "score";

} // namespace

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<AlarmStatus>::convert(const AlarmStatus& alarmStatus)
{
    return QJsonObject{
        {levelTag, json::toValue(alarmStatus.level)},
        {scoreTag, json::toValue(alarmStatus.score)}};
}

AlarmStatus FromJsonConverter<AlarmStatus>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .level =
            json::fromObject<decltype(AlarmStatus::level)>(object, levelTag),
        .score =
            json::fromObject<decltype(AlarmStatus::score)>(object, scoreTag)};
}
