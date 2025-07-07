#pragma once

#include "exception/InvalidValue.h"

#include <yaml-cpp/node/node.h>

namespace mpk::dss::core
{

template <typename T>
T convertYAML(const YAML::Node& yaml)
{
    auto res = T{};
    auto isConverted = YAML::convert<T>::decode(yaml, res);

    if (!isConverted)
    {
        BOOST_THROW_EXCEPTION(
            exception::InvalidValue() << exception::ExceptionInfo(
                std::string("Conversion error: ") + yaml.Scalar()));
    }
    return res;
}

} // namespace mpk::dss::core
