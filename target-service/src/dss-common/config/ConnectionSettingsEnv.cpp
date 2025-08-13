#include "ConnectionSettingsEnv.h"

#include "dss-backend/exceptions/InvalidConfig.h"
#include "dss-backend/exceptions/InvalidConfigValue.h"
#include "dss-common/config/Config.h"

#include <boost/throw_exception.hpp>

#include <cstdio>
#include <cstdlib>

namespace mpk::dss::config
{

ConnectionSettingsEnv::ConnectionSettingsEnv(const std::string& envVarName)
{
    auto* envValue = std::getenv(envVarName.c_str());
    if (nullptr == envValue)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(envVarName));
    }
    std::string value = envValue;
    auto pos = value.find(':');
    if (pos == std::string::npos)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfig{}
            << exception::ExceptionInfo(value));
    }
    host = QString::fromStdString(value.substr(0, pos));

    auto portStr = value.substr(pos + 1, value.length());
    std::istringstream parser(portStr);
    parser >> port;
    if (parser.fail() || !parser.eof())
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfig{}
            << exception::ExceptionInfo("Invalid port value: " + portStr));
    }
}

} // namespace mpk::dss::config
