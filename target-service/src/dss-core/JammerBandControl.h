#pragma once

#include "dss-core/JammerBand.h"
#include "dss-core/JammerBandOption.h"
#include "dss-core/JammerBandSignals.h"
#include "dss-core/JammerMode.h"

#include "gsl/pointers"

namespace mpk::dss::core
{

class JammerBandSignals;

class JammerBandControl
{
public:
    virtual ~JammerBandControl() = default;

    [[nodiscard]] virtual JammerBands bands() = 0;
    virtual bool setBands(const JammerBandOption& bandsActive) = 0;
    virtual void unsetAllBands()
    {
        setBands(JammerBandOption());
    }

    [[nodiscard]] virtual JammerBandOption maxBands()
    {
        auto b = bands();
        return toBandOptionFull(b.begin(), b.end());
    }

    [[nodiscard]] virtual gsl::not_null<const JammerBandSignals*> bandSignals()
        const = 0;

    [[nodiscard]] virtual core::JammerBandOptionVec bandOptions()
    {
        return {};
    }

    [[nodiscard]] virtual core::JammerMode::Value jammerMode() const = 0;
};

} // namespace mpk::dss::core
