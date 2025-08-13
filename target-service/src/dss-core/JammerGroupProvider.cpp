#include "JammerGroupProvider.h"

#include "dss-backend/exceptions/InvalidConfig.h"

#include "boost/iterators/details/ValueTypeForwardIterator.h"

namespace mpk::dss::core
{

constexpr auto jammerGroupsVarName = "GROUP_JAMMER_DEVICES";

JammerGroupProvider::JammerGroupProvider()
{
    auto* jammerGroupsValue = std::getenv(jammerGroupsVarName);
    if (jammerGroupsValue != nullptr)
    {
        auto jammerGroupsStr = QString::fromUtf8(jammerGroupsValue);
        auto jammerGroups = jammerGroupsStr.split(' ');
        for (const auto& jammerGroup: jammerGroups)
        {
            auto groupData = jammerGroup.split('#');
            if (groupData.size() != 2)
            {
                BOOST_THROW_EXCEPTION(
                    dss::backend::exception::InvalidConfig{}
                    << exception::ExceptionInfo(jammerGroup.toStdString()));
            }
            OAService::OAServiceJammer_group group;
            group.setId(groupData.at(0));
            auto name = groupData.at(1);
            group.setName(name.replace('_', ' '));
            m_groups.emplace_back(group);
        }
    }
}

using PrivateIterator = mpkit::details::ValueTypeForwardIterator<
    std::vector<OAService::OAServiceJammer_group>::const_iterator,
    OAService::OAServiceJammer_group const>;

auto JammerGroupProvider::begin() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_groups.begin())};
}

auto JammerGroupProvider::end() const -> Iterator
{
    return Iterator{std::make_shared<PrivateIterator>(m_groups.end())};
}

auto JammerGroupProvider::find(const QString& groupId) const -> Iterator
{
    auto it = std::find_if(
        m_groups.begin(),
        m_groups.end(),
        [groupId](const auto& group) { return group.getId() == groupId; });
    return Iterator{std::make_shared<PrivateIterator>(it)};
}

bool JammerGroupProvider::contains(const QString& groupId) const
{
    return find(groupId) != end();
}

} // namespace mpk::dss::core
