#pragma once

#include "types/StrongTypedef.h"
#include "types/strong_typedef/json/Traits.hpp"

#include <QString>

namespace mpk::dss::core
{

class DeviceId
  : public types::StrongTypedef<DeviceId, types::TypeIdentity<QString>>,
    public types::SerializableAsJsonValue<DeviceId>,
    public types::Streamable<DeviceId, std::ostream&>
{
    using StrongTypedef::StrongTypedef;

public:
    [[nodiscard]] bool valid() const noexcept
    {
        return !value().isEmpty();
    }

    static DeviceId generate()
    {
        return DeviceId{QUuid::createUuid().toString(QUuid::WithoutBraces)};
    }
};

} // namespace mpk::dss::core

template <>
struct std::hash<mpk::dss::core::DeviceId>
{
    std::size_t operator()(
        const mpk::dss::core::DeviceId& deviceId) const noexcept
    {
        return std::hash<std::string>{}(
            static_cast<QString>(deviceId).toStdString());
    }
};
