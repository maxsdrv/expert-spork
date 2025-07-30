#include "mpk/stable_link/PeriodicalPolicy.h"

namespace mpk::stable_link
{

PeriodicalPolicy::PeriodicalPolicy(const PolicySettings& settings) noexcept : m_settings{settings}
{
}

void PeriodicalPolicy::nextConnectingTry()
{
    if (!m_settings.connectingTries || m_connectingTry < m_settings.connectingTries)
    {
        ++m_connectingTry;
    }
}

void PeriodicalPolicy::resetConnectingTries()
{
    m_connectingTry = 0;
}

Policy::Delay PeriodicalPolicy::connectingDelay() const
{
    if (!m_settings.connectingTries || m_connectingTry < m_settings.connectingTries)
    {
        return m_settings.connectingDelay;
    }

    return {};
}

Policy::Timeout PeriodicalPolicy::connectingTimeout() const
{
    return m_settings.connectingTimeout;
}

Policy::Timeout PeriodicalPolicy::respondingTimeout() const
{
    return m_settings.respondingTimeout;
}

Policy::Action PeriodicalPolicy::notRespondingAction() const
{
    return m_settings.notRespondingAction;
}

} // namespace mpk::stable_link
