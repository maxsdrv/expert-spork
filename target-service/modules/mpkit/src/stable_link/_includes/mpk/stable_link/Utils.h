#pragma once

#include "mpk/stable_link/Link.h"

#include <QEventLoop>

namespace mpk::stable_link
{

bool waitFor(
    const Link& link,
    State state,
    QEventLoop::ProcessEventsFlag processEventsFlag,
    std::chrono::milliseconds timeout);

bool waitFor(const Link& link, State state, QEventLoop::ProcessEventsFlag processEventsFlag);

bool waitFor(const Link& link, State state);

} // namespace mpk::stable_link
