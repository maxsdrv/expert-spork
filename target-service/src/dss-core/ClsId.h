#pragma once

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

#include <vector>

namespace mpk::dss::core
{

class ClsId : public types::StrongTypedef<ClsId, types::TypeIdentity<QString>>,
              public types::SerializableAsJsonValue<ClsId>
{
    using StrongTypedef::StrongTypedef;
};
using ClsIds = std::vector<ClsId>;

} // namespace mpk::dss::core
