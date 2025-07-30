#pragma once

#include "metric_units/MetricUnits.h"

namespace mpk::metric_units
{

struct DecibelTraits
{
    using ValueType = double;
    using Prefix = metric_units::prefix::None;
    using PrefixList = metric_units::prefix::ListNone;
    static constexpr auto name = QT_TRANSLATE_NOOP("mpk::metric_units", "dB");
    static inline const auto numericValidate = &metric_units::numeric_validator::allDouble;
    static constexpr bool implicitConstructor = true;
};

// Decibels is a special kind of unit.
// It is mathematically "metric" (Additive, Scalable), so it is convenient
// to define it as real metric with special traits
using Decibel = metric_units::MetricUnit<DecibelTraits>;

} // namespace mpk::metric_units
