#pragma once

#include "dss-backend/BackendControllerBase.h"

#include <yaml-cpp/yaml.h>

namespace mpk::dss::backend::alia
{

constexpr auto nodeKey = "alia";

class AliaController : public BackendControllerBase
{

public:
    static BackendControllerPtr fromDescription(
        const YAML::Node& description, const BackendComponents& components);

private:
    explicit AliaController(const BackendComponents& components);
};

} // namespace mpk::dss::backend::alia
