#include "Frequency.h"

namespace mpk::drone::core
{

namespace
{

constexpr auto kHzQuantifier = 1000;

} // namespace

double convertFromKHz(Frequency frequency)
{
    return static_cast<double>(frequency) * kHzQuantifier;
}

Frequency convertToKHz(double frequency)
{
    return static_cast<Frequency>(frequency / kHzQuantifier);
}

} // namespace mpk::drone::core

namespace mpk::signal_scanner::detector
{

bool operator==(const FreqInterval& left, const FreqInterval& right)
{
    return left.begin() == right.begin() && left.end() == right.end();
}
bool operator!=(const FreqInterval& left, const FreqInterval& right)
{
    return !(left == right);
}

} // namespace mpk::signal_scanner::detector
