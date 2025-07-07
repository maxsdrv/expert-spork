#pragma once

#include "dss-core/DeviceHwInfo.h"
#include "dss-core/DeviceId.h"
#include "dss-core/DevicePosition.h"
#include "dss-core/DeviceState.h"
#include "dss-core/DeviceWorkzone.h"
#include "dss-core/JammerBand.h"
#include "dss-core/JammerBandOption.h"
#include "dss-core/JammerDevice.h"
#include "dss-core/JammerGroupControl.h"
#include "dss-core/JammerTimeoutStatus.h"

#include <optional>

namespace mpk::dss::core
{

struct JammerInfo
{
    DeviceId id;
    QString model;
    QString serial;
    QString swVersion;
    bool disabled;
    DeviceState::Value state;
    DevicePosition position;
    DevicePositionMode positionMode;
    DeviceWorkzone workzone;
    DeviceId sensorId;
    JammerBands bands;
    core::JammerBandOptionVec bandOptions;
    std::optional<JammerTimeoutStatus> timeoutStatus;
    std::optional<QString> groupId;
    DeviceHwInfo hwInfo;

    static JammerInfo fromDevice(const JammerDevice& device);
};

} // namespace mpk::dss::core
