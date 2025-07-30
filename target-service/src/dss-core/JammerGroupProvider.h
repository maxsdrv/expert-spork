#pragma once

#include "OAServiceJammer_group.h"

#include "boost/iterators/ForwardIterator.h"

#include <vector>

namespace mpk::dss::core
{

class JammerGroupProvider
{
public:
    explicit JammerGroupProvider();

    using Iterator =
        mpkit::ForwardIterator<OAService::OAServiceJammer_group const>;

    [[nodiscard]] Iterator begin() const;
    [[nodiscard]] Iterator end() const;
    [[nodiscard]] Iterator find(const QString& groupId) const;
    [[nodiscard]] bool contains(const QString& groupId) const;

private:
    std::vector<OAService::OAServiceJammer_group> m_groups;
};

} // namespace mpk::dss::core
