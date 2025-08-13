#include "ConfigParser.h"

#include "dss-backend/BackendControllerBase.h"

#include "exception/KeyInfo.h"
#include "exceptions/DuplicateParserKey.h"
#include "exceptions/InvalidConfig.h"
#include "exceptions/MissingParser.h"

namespace mpk::dss::backend
{

void BackendFactory::registerNodeParser(const NodeKey& key, NodeParser parser)
{
    auto it = m_parsers.find(key);
    if (it == m_parsers.end())
    {
        m_parsers.insert({key, std::move(parser)});
        return;
    }

    BOOST_THROW_EXCEPTION(
        exception::DuplicateParserKey{} << ::exception::KeyInfo{key});
}

auto BackendFactory::createBackends(
    const YAML::Node& config, const BackendComponents& components)
    -> BackendList
{
    constexpr auto modulesTag = "modules";

    auto modulesNode = config[modulesTag];
    if (modulesNode.IsNull() || modulesNode.size() == 0)
    {
        BOOST_THROW_EXCEPTION(
            exception::InvalidConfig{}
            << ::exception::ExceptionInfo("no valid modules"));
    }

    auto backends = BackendList{};
    for (const auto& moduleNode: modulesNode)
    {
        for (const auto& pair: moduleNode)
        {
            auto key = pair.first.as<std::string>();
            auto it = m_parsers.find(key);
            if (it != m_parsers.end())
            {
                auto backendPtr = it->second(pair.second, components);
                backends.push_back(std::move(backendPtr));
            }
            else
            {
                BOOST_THROW_EXCEPTION(
                    exception::MissingParser{} << ::exception::KeyInfo{key});
            }
        }
    }
    return backends;
}

} // namespace mpk::dss::backend
