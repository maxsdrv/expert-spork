#include "httpws/yaml/ConnectionSettings.h"

namespace YAML
{

Node convert<http::ConnectionSettings>::encode(const http::ConnectionSettings& settings)
{
    using Tag = http::ConnectionSettings::Tag;
    Node node;
    node[Tag::host] = settings.host;
    node[Tag::port] = settings.port;
    return node;
}

bool convert<http::ConnectionSettings>::decode(
    const Node& node, http::ConnectionSettings& settings)
{
    using Tag = http::ConnectionSettings::Tag;
    settings.host = node[Tag::host].as<QString>();
    settings.port = node[Tag::port].as<quint16>();
    return true;
}

} // namespace YAML
