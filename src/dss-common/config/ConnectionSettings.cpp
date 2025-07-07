#include "ConnectionSettings.h"

#include "Config.h"

#include <string>

namespace YAML
{

bool convert<mpk::dss::config::ConnectionSettings>::decode(
    const Node& node, mpk::dss::config::ConnectionSettings& cs)
{
    using Tag = http::ConnectionSettings::Tag;

    std::string host;
    if (!parseValue(node[Tag::host], host))
    {
        return false;
    }
    cs.host = QString::fromStdString(host);

    int port = {};
    if (!parseValue(node[Tag::port], port))
    {
        return false;
    }
    cs.port = port;

    return true;
}

} // namespace YAML
