#include "DeviceHwInfoVsJson.h"

#include <QJsonValueRef>

using namespace mpk::dss::core;

QJsonValue ToJsonConverter<DeviceHwInfo>::convert(const DeviceHwInfo& hwInfo)
{
    auto object = QJsonObject{};
    for (const auto& [tag, value]: hwInfo.values)
    {
        object.insert(tag, value);
    }
    return object;
}

DeviceHwInfo FromJsonConverter<DeviceHwInfo>::get(const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    auto values = decltype(DeviceHwInfo::values){};
    for (auto it = object.begin(); it != object.end(); ++it)
    {
        values.emplace(it.key(), it.value().toString());
    }
    return {values};
}
