#pragma once

#include "mpk/rest/SynchronizerSignals.h"

#include <gsl/pointers>

namespace mpk::rest
{

class Synchronizer
{
public:
    using SynchronizerSignalsPin = gsl::not_null<const SynchronizerSignals*>;

    virtual ~Synchronizer() = default;

    virtual void sync() = 0;
    virtual bool synced() const = 0;

    virtual SynchronizerSignalsPin synchronizerSignals() const = 0;
};

} // namespace mpk::rest
