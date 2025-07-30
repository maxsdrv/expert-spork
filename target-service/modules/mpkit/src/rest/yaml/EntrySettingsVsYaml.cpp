#include "mpk/rest/yaml/EntrySettingsVsYaml.h"

#include "mpk/rest/tag/EntrySettingsTag.h"

#include "httpws/yaml/ConnectionSettings.h"

using EntrySettings = mpk::rest::EntrySettings;
using Tag = mpk::rest::EntrySettingsTag;

namespace YAML
{

Node convert<EntrySettings>::encode(const EntrySettings& value)
{
    Node node;

    if (value.httpServer.has_value())
    {
        node[Tag::httpServer] = value.httpServer.value();
    }

    if (value.broadcastServer.has_value())
    {
        node[Tag::broadcastServer] = value.broadcastServer.value();
    }

    return node;
}

bool convert<EntrySettings>::decode(const Node& node, EntrySettings& value)
{
    if (auto httpServerNode = node[Tag::httpServer])
    {
        value.httpServer = httpServerNode.as<EntrySettings::ConnectionSettings>();
    }

    if (auto broadcastServerNode = node[Tag::broadcastServer])
    {
        value.broadcastServer = broadcastServerNode.as<EntrySettings::ConnectionSettings>();
    }

    return true;
}

} // namespace YAML
