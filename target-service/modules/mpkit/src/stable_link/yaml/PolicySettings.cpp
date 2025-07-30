#include "mpk/stable_link/yaml/PolicySettings.h"

namespace
{

auto constexpr connectingTriesTag = "connecting_tries";
auto constexpr connectingDelayTag = "connecting_delay";
auto constexpr connectingTimeoutTag = "connecting_timeout";
auto constexpr respondingTimeoutTag = "responding_timeout";

} // namespace

namespace YAML
{

Node convert<mpk::stable_link::PolicySettings>::encode(const PolicySettings& policy)
{
    Node node;
    node[connectingTriesTag] = policy.connectingTries;
    node[connectingDelayTag] = policy.connectingDelay;
    node[connectingTimeoutTag] = policy.connectingTimeout;
    node[respondingTimeoutTag] = policy.respondingTimeout;
    return node;
}

bool convert<mpk::stable_link::PolicySettings>::decode(const Node& node, PolicySettings& policy)
{
    policy.connectingTries = node[connectingTriesTag].as<PolicySettings::Tries>();
    policy.connectingDelay = node[connectingDelayTag].as<PolicySettings::Duration>();
    policy.connectingTimeout = node[connectingTimeoutTag].as<PolicySettings::Timeout>();
    policy.respondingTimeout = node[respondingTimeoutTag].as<PolicySettings::Timeout>();
    policy.notRespondingAction = mpk::stable_link::PolicySettings{}.notRespondingAction;
    return true;
}

} // namespace YAML
