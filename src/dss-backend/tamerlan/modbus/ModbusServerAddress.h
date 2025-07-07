#pragma once

#include "types/StrongTypedef.h"

namespace mpk::dss::backend::tamerlan
{

class ModbusServerAddress
  : public types::StrongTypedef<ModbusServerAddress, types::TypeIdentity<int>>
{
    using StrongTypedef::StrongTypedef;
};

} // namespace mpk::dss::backend::tamerlan
