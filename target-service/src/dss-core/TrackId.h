#pragma once

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

#include <vector>

namespace mpk::dss::core
{

class TrackId
  : public types::StrongTypedef<TrackId, types::TypeIdentity<QString>>,
    public types::SerializableAsJsonValue<TrackId>,
    public types::Streamable<TrackId, std::ostream&>
{
    using StrongTypedef::StrongTypedef;

public:
    static TrackId generate()
    {
        return TrackId{QUuid::createUuid().toString(QUuid::WithoutBraces)};
    }
};
using TrackIds = std::vector<TrackId>;

} // namespace mpk::dss::core
