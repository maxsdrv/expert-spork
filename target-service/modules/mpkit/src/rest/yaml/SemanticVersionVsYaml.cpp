#include "mpk/rest/yaml/SemanticVersionVsYaml.h"

#include "mpk/rest/tag/SemanticVersionTag.h"

using SemanticVersion = mpk::rest::SemanticVersion;
using Tag = mpk::rest::SemanticVersionTag;

namespace YAML
{

Node convert<SemanticVersion>::encode(const SemanticVersion& version)
{
    Node node;
    node[Tag::major] = version.major;
    node[Tag::minor] = version.minor;
    node[Tag::patch] = version.patch;
    return node;
}

bool convert<SemanticVersion>::decode(const Node& node, SemanticVersion& version)
{
    version.major = node[Tag::major].as<unsigned>();
    version.minor = node[Tag::minor].as<unsigned>();
    version.patch = node[Tag::patch].as<unsigned>();

    return true;
}

} // namespace YAML
