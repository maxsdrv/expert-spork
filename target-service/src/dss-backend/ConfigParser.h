#pragma once

#include "dss-backend/BackendController.h"

#include <yaml-cpp/yaml.h>

#include <functional>
#include <string>

namespace mpk::dss::backend
{

struct BackendComponents;

class BackendFactory
{
public:
    using NodeKey = std::string;
    using BackendList = std::vector<BackendControllerPtr>;
    using NodeParser = std::function<BackendControllerPtr(
        const YAML::Node& node, const BackendComponents& components)>;

    void registerNodeParser(const NodeKey& key, NodeParser parser);
    BackendList createBackends(
        const YAML::Node& config, const BackendComponents& components);

private:
    std::map<NodeKey, NodeParser> m_parsers;
};

} // namespace mpk::dss::backend
