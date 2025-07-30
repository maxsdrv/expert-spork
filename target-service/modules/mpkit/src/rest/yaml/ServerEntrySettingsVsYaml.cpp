#include "mpk/rest/yaml/ServerEntrySettingsVsYaml.h"

#include "mpk/rest/SemanticVersionUtility.h"
#include "mpk/rest/tag/ServerEntrySettingsTag.h"

using ServerEntrySettings = mpk::rest::ServerEntrySettings;
using SemanticVersionUtility = mpk::rest::SemanticVersionUtility;
using Tag = mpk::rest::ServerEntrySettingsTag;

namespace YAML
{

Node convert<ServerEntrySettings>::encode(const ServerEntrySettings& settings)
{
    Node node;
    node[Tag::address] = settings.httpAddress.host;
    node[Tag::httpPort] = settings.httpAddress.port;

    if (settings.broadcastAddress.has_value())
    {
        node[Tag::broadcastPort] = settings.broadcastAddress->port;
    }

    if (settings.requiredVersion.has_value())
    {
        node[Tag::requiredVersion] = SemanticVersionUtility::toString(*settings.requiredVersion);
    }

    return node;
}

bool convert<ServerEntrySettings>::decode(const Node& node, ServerEntrySettings& settings)
{
    auto addressNode = node[Tag::address];
    settings.httpAddress.host = addressNode[Tag::host].as<QString>();
    settings.httpAddress.port = addressNode[Tag::httpPort].as<int>();

    if (auto broadcastPortNode = addressNode[Tag::broadcastPort])
    {
        settings.broadcastAddress = settings.httpAddress;
        settings.broadcastAddress->port = broadcastPortNode.as<int>();
    }

    settings.login = node[Tag::login].as<QString>();

    if (auto compatibilityNode = node[Tag::requiredVersion])
    {
        auto versionStr = compatibilityNode.as<std::string>();
        settings.requiredVersion = SemanticVersionUtility::fromString(versionStr);
    }

    return true;
}

} // namespace YAML
