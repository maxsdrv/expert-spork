#pragma once

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

namespace mpk::dss::core
{

class TargetId
  : public types::StrongTypedef<TargetId, types::TypeIdentity<QString>>,
    public types::SerializableAsJsonValue<TargetId>,
    public types::Streamable<TargetId, std::ostream&>
{
public:
    using StrongTypedef::StrongTypedef;

    static TargetId generate()
    {
        return TargetId{QUuid::createUuid().toString(QUuid::WithoutBraces)};
    }
};

} // namespace mpk::dss::core
