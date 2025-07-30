#pragma once

#include <chrono>
#include <optional>

namespace mpk::stable_link
{

class Policy
{
public:
    using Tries = std::optional<std::size_t>;
    using Duration = std::chrono::milliseconds;
    using Delay = std::optional<Duration>;
    using Timeout = std::optional<Duration>;

    enum class Action : std::uint8_t
    {
        Reconnect,
        Disconnect
    };

    virtual ~Policy() = default;

    /**
     * Iterates to the next connection attempt in internal logic.
     */
    virtual void nextConnectingTry() = 0;

    /**
     * Resets connecting tries in the internal logic.
     */
    virtual void resetConnectingTries() = 0;

    /**
     * Returns the delay between connecting tries.
     */
    virtual Delay connectingDelay() const = 0;

    /**
     * Returns the timeout to interrupt a protracted connecting.
     */
    virtual Timeout connectingTimeout() const = 0;

    /**
     * Returns the timeout to detect the 'notResponding' state.
     */
    virtual Timeout respondingTimeout() const = 0;

    /**
     * Returns the action to react to the 'notResponding' state.
     */
    virtual Action notRespondingAction() const = 0;
};

} // namespace mpk::stable_link
