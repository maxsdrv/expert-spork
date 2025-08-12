#pragma once

#include "ModbusDataUnitsDef.h"
#include "SensorChannelParameters.h"

#include <optional>

namespace mpk::dss::imitator::tamerlan::sensor
{

class Model
{
public:
    [[nodiscard]] static ModbusDataUnits createModbusDataUnits(
        const std::optional<SensorChannelsParameters>& channelsParameters =
            std::nullopt);
};

} // namespace mpk::dss::imitator::tamerlan::sensor
