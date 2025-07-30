#include "dss-backend/exceptions/InvalidConfigValue.h"

#include <boost/throw_exception.hpp>

#include <cstdlib>
#include <string>

namespace mpk::dss::config
{

std::string expandEnvVar(const std::string& value)
{
    if (value.empty() || value == "null")
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(std::string("Empty value")));
    }
    if (value[0] == '$')
    {
        auto* envValue = std::getenv(value.substr(1, value.size() - 1).c_str());
        if (nullptr == envValue)
        {
            BOOST_THROW_EXCEPTION(
                dss::backend::exception::InvalidConfigValue{}
                << exception::ExceptionInfo(value));
        }
        return envValue;
    }
    return value;
}

} // namespace mpk::dss::config
