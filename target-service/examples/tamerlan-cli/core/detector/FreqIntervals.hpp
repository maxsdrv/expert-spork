#pragma once

#include <cstdint>
#include <vector>

#include "IntervalInt.hpp"

/*
    Module for storing operating ranges and returning the current frequency
    Input parameter: the band of the radio receiving path in
    class is very simple now
*/

namespace mpk::signal_scanner::detector
{
class FreqInterval : public IntervalInt
{
public:
    FreqInterval() = default;
    FreqInterval(int32_t begin, int32_t end, int32_t band) :
      IntervalInt(begin, end), m_band(band){};
    ~FreqInterval() override = default;
    void clear() override
    {
        IntervalInt::clear();
        m_band = 0;
    };

    [[nodiscard]] int32_t band() const
    {
        return m_band;
    }

private:
    int32_t m_band = 0;
};

class FreqIntervals
{
public:
    FreqIntervals() = default;

    void addInterval(int32_t begin, int32_t end, int32_t band);
    void clear();

    void restart();
    [[nodiscard]] int32_t getFrequency();

private:
    std::vector<FreqInterval> m_freqIntervals;
    int32_t m_currentFreq = 0;
    int32_t m_currentRange = 0;

    void setBeginFreqFromRange();
};
} // namespace mpk::signal_scanner::detector