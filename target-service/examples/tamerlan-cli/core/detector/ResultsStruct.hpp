#pragma once

#include <cstdint>

#include "IntervalInt.hpp"

namespace mpk::signal_scanner::detector
{

// for future
enum class Modulation
{
    Unrecognize,
    NN,
    Chirp,
    PM,
    FM2,
    OFDM
};

class ModulationStruct
{
public:
    ModulationStruct(
        Modulation modulation,
        float probability,
        float freqOffsetKHz,
        float speedKHz,
        float mainParam) :
      m_modulation(modulation),
      m_probability(probability),
      m_freqOffsetKHz(freqOffsetKHz),
      m_speedKHz(speedKHz),
      m_mainParam(mainParam)
    {
    }

    ModulationStruct() = default;

    [[nodiscard]] Modulation modulation() const
    {
        return m_modulation;
    };
    void setModulation(Modulation mod)
    {
        m_modulation = mod;
    }
    [[nodiscard]] float probability() const
    {
        return m_probability;
    };
    void setProbability(float probability)
    {
        m_probability = probability;
    }
    [[nodiscard]] float freqOffsetKHz() const
    {
        return m_freqOffsetKHz;
    };
    void setFreqOffsetKHz(float freqOffsetKHz)
    {
        m_freqOffsetKHz = freqOffsetKHz;
    }
    [[nodiscard]] float speedKHz() const
    {
        return m_speedKHz;
    };
    void setSpeedKHz(float freqOffsetKHz)
    {
        m_freqOffsetKHz = freqOffsetKHz;
    }
    [[nodiscard]] float mainParam() const
    {
        return m_mainParam;
    };
    void setMainParam(float mainParam)
    {
        m_mainParam = mainParam;
    }

    void reset()
    {
        m_modulation = Modulation::Unrecognize;
        m_probability = 0.0F;
        m_freqOffsetKHz = 0.0F;
        m_speedKHz = 0.0F;
        m_mainParam = 0.0F;
    };

private:
    Modulation m_modulation = Modulation::Unrecognize;
    float m_probability = 0.0F;
    float m_freqOffsetKHz = 0.0F;
    float m_speedKHz = 0.0F;
    float m_mainParam = 0.0F;
};

struct ResultsStruct
{
    int32_t centralFreqKHz = 0;
    int32_t wideFreqKHz = 0;
    int64_t beginMKS = 0;
    int32_t lengthMKC = 0;
    // int32_t type;
    ModulationStruct modulation;
    // drone identification number
    int32_t identificationNumber = -1;
};

// struct averaging results, use for unlinked results
struct AverageResults
{
    IntervalInt range;    // location of results
    int32_t count;        // results counter
    int32_t middleWide;   // in kHz
    int32_t middleLength; // in mkC
};

}; // namespace mpk::signal_scanner::detector