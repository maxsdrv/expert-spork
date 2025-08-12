#pragma once

#include "JammerChannelParameters.h"
#include "ModbusDataUnitsDef.h"

#include <optional>

namespace mpk::dss::imitator::tamerlan::jammer
{

class Model
{
public:
    [[nodiscard]] static ModbusDataUnits createModbusDataUnits(
        const std::optional<JammerChannelsParameters>& channelsParameters =
            std::nullopt);
};

} // namespace mpk::dss::imitator::tamerlan::jammer
