#pragma once

#include "mpk/stable_link/Policy.h"

namespace mpk::stable_link
{

struct PolicySettings
{
    using Tries = Policy::Tries;
    using Duration = Policy::Duration;
    using Timeout = Policy::Timeout;
    using Action = Policy::Action;

    /**
     * The number of tries to connect.
     * An empty value indicates an infinite number of tries.
     */
    Tries connectingTries = 3;
    /**
     * The delay between connecting tries.
     */
    Duration connectingDelay = std::chrono::seconds(1);
    /**
     * The timeout to interrupt a protracted connecting.
     * An empty value instructs not to interrupt the connection.
     */
    Timeout connectingTimeout = std::chrono::seconds(2);
    /**
     * The timeout to detect the 'notResponding' state.
     * An empty value instructs not to detect the 'notResponding' state.
     */
    Timeout respondingTimeout = std::chrono::seconds(2);
    /**
     * The action to react to the 'notResponding' state.
     */
    Action notRespondingAction = Action::Reconnect;
};

} // namespace mpk::stable_link
