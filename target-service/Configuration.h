#pragma once

#include <yaml-cpp/yaml.h>

#include "dss-backend/BackendController.h"

#include <memory>
#include <vector>

namespace mpk::dss::backend
{

struct BackendComponents;

std::vector<std::unique_ptr<BackendController>> createBackend(
    const YAML::Node& config, const BackendComponents& components);

} // namespace mpk::dss::backend
