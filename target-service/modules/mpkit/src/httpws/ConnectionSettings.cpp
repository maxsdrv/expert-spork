#include "httpws/ConnectionSettings.h"

#include <tuple>

namespace http
{

bool ConnectionSettings::operator==(const ConnectionSettings& rhs) const
{
    return std::tie(host, port) == std::tie(rhs.host, rhs.port);
}

bool ConnectionSettings::operator!=(const ConnectionSettings& rhs) const
{
    return !(*this == rhs);
}

bool ConnectionSettings::isValid(const ConnectionSettings& settings)
{
    return !settings.host.isEmpty() && settings.port > 0;
}

} // namespace http
