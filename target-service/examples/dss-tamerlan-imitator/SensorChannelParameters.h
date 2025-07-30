#pragma once

#include "modbus/device/ChannelDataModbus.h"

#include <QtGlobal>

#include <string>
#include <vector>

namespace mpk::dss::imitator::tamerlan
{

enum class SensorStatus : quint16
{
    NoAlarm = 0,
    Alarm = 1,
    Error = 0x80F0
};

struct SensorChannelParameter
{
    SensorStatus status{SensorStatus::NoAlarm};
    ChannelZoneModbus channelZone;
};

using SensorChannelsParameters = std::vector<SensorChannelParameter>;

} // namespace mpk::dss::imitator::tamerlan
