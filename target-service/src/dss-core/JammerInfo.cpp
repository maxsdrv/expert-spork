#include "JammerInfo.h"

#include "JammerDevice.h"
#include "JammerProperties.h"

namespace mpk::dss::core
{

JammerInfo JammerInfo::fromDevice(const JammerDevice& device)
{
    return {
        .id = device.id(),
        .model = device.properties().model(),
        .serial = device.properties().serial(),
        .swVersion = device.properties().swVersion(),
        .disabled = device.properties().disabled(),
        .state = device.properties().state(),
        .position = device.properties().position(),
        .positionMode = device.properties().positionMode(),
        .workzone = device.properties().workzone(),
        .sensorId = device.properties().sensorId(),
        .bands = device.properties().bands(),
        .bandOptions = device.properties().bandOptions(),
        .timeoutStatus = device.properties().timeoutControl().status(),
        .groupId = device.properties().groupId(),
        .hwInfo = device.properties().hwInfo()};
}

} // namespace mpk::dss::core
