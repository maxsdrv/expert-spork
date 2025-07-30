#pragma once

#include "mpk/stable_link/Signals.h"
#include "mpk/stable_link/State.h"

#include <gsl/pointers>

namespace mpk::stable_link
{

class Informer
{
public:
    using LinkState = State;
    using LinkSignals = Signals;
    using LinkSignalsPin = gsl::not_null<const LinkSignals*>;

    virtual ~Informer() = default;

    virtual LinkState linkState() const = 0;

    virtual LinkSignalsPin linkSignals() const = 0;
};

} // namespace mpk::stable_link
