#include "DiagnosticDataVsJson.h"

namespace
{

constexpr auto stateTag = "state";
constexpr auto cpuTempTag = "cpu_temp";

} // namespace

using namespace mpk::dss::backend::rest;

QJsonValue ToJsonConverter<DeviceDiagnosticData>::convert(
    const DeviceDiagnosticData& data)
{
    return QJsonObject{
        {stateTag, json::toValue(data.state)},
        {cpuTempTag, json::toValue(data.cpuTemp)}};
}

DeviceDiagnosticData FromJsonConverter<DeviceDiagnosticData>::get(
    const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    return {
        .state = json::fromObject<decltype(DeviceDiagnosticData::state)>(
            object, stateTag),
        .cpuTemp = json::fromObject<decltype(DeviceDiagnosticData::cpuTemp)>(
            object, cpuTempTag)};
}
