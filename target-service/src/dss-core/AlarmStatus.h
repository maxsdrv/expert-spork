#pragma once

#include "dss-core/AlarmLevel.h"

namespace mpk::dss::core
{

struct AlarmStatus
{
    AlarmLevel::Value level;
    double score;
};

} // namespace mpk::dss::core
