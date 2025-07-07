#pragma once

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

#include <vector>

namespace mpk::dss::core
{

class AlarmId
  : public types::StrongTypedef<AlarmId, types::TypeIdentity<QString>>,
    public types::SerializableAsJsonValue<AlarmId>
{
    using StrongTypedef::StrongTypedef;
};
using AlarmIds = std::vector<AlarmId>;

} // namespace mpk::dss::core
