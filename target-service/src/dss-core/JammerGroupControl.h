#pragma once

#include <QString>

#include <optional>

namespace mpk::dss::core
{

class JammerGroupControl
{
public:
    virtual ~JammerGroupControl() = default;

    using GroupId = QString;

    [[nodiscard]] virtual std::optional<GroupId> groupId() const = 0;
};

} // namespace mpk::dss::core
