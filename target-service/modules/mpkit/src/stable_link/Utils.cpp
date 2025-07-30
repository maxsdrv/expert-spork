#include "mpk/stable_link/Utils.h"

#include <QTimer>

#include <chrono>

namespace mpk::stable_link
{

bool waitFor(
    const Link& link,
    State state,
    QEventLoop::ProcessEventsFlag processEventsFlag,
    std::chrono::milliseconds timeout)
{
    if (link.linkState() != state)
    {
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        QObject::connect(
            link.linkSignals(),
            &Signals::linkStateChanged,
            &loop,
            [&loop, state](const auto& newState)
            {
                if (state == newState)
                {
                    loop.quit();
                }
            });
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(timeout);
        loop.exec(processEventsFlag);
        return timer.isActive();
    }
    return true;
}

bool waitFor(const Link& link, State state, QEventLoop::ProcessEventsFlag processEventsFlag)
{
    using namespace std::chrono_literals;
    const auto defaultTimeout = 30s;

    return waitFor(link, state, processEventsFlag, defaultTimeout);
}

bool waitFor(const Link& link, State state)
{
    return waitFor(link, state, QEventLoop::ExcludeUserInputEvents);
}

} // namespace mpk::stable_link
