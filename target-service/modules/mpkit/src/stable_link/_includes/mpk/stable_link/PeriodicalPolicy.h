#pragma once

#include "mpk/stable_link/PolicySettings.h"

namespace mpk::stable_link
{

class PeriodicalPolicy : public Policy
{
public:
    explicit PeriodicalPolicy(const PolicySettings& settings) noexcept;

    void nextConnectingTry() override;
    void resetConnectingTries() override;

    Delay connectingDelay() const override;
    Timeout connectingTimeout() const override;
    Timeout respondingTimeout() const override;
    Action notRespondingAction() const override;

private:
    std::size_t m_connectingTry{0};
    PolicySettings m_settings;
};

} // namespace mpk::stable_link
