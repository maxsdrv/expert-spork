#pragma once

#include "detector/CommonTypes.hpp"
#include "detector/FreqIntervals.hpp"

namespace mpk::drone::core
{

using Frequency = signal_scanner::detector::FrequencyKHz;
using FrequencyRange = signal_scanner::detector::FreqInterval;

double convertFromKHz(Frequency frequency);
Frequency convertToKHz(double frequency);

} // namespace mpk::drone::core

namespace mpk::signal_scanner::detector
{

bool operator==(const FreqInterval& left, const FreqInterval& right);
bool operator!=(const FreqInterval& left, const FreqInterval& right);

} // namespace mpk::signal_scanner::detector
