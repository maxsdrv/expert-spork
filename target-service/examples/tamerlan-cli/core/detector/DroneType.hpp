#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "IntervalInt.hpp"
#include "ResultsStruct.hpp"

namespace mpk::signal_scanner::detector
{

class ModulationChecker
{
public:
    ModulationChecker(
        const Modulation& mod,
        const IntervalInt& speed,
        const IntervalInt& mainParam) :
      m_mod(mod),
      m_modDefault(m_mod == Modulation::Unrecognize),
      m_speed(speed),
      m_speedDefault(0 == m_speed.begin() && 0 == m_speed.end()),
      m_mainParam(mainParam),
      m_mainParamDefault(0 == m_mainParam.begin() && 0 == m_mainParam.end())
    {
    }

    explicit ModulationChecker(const Modulation& mod) :
      m_mod(mod),
      m_modDefault(m_mod == Modulation::Unrecognize),
      m_speed(IntervalInt(0, 0)),
      m_speedDefault(true),
      m_mainParam(IntervalInt(0, 0)),
      m_mainParamDefault(true)
    {
    }

    ModulationChecker() :
      m_mod(Modulation::Unrecognize),
      m_modDefault(true),
      m_speed(IntervalInt(0, 0)),
      m_speedDefault(true),
      m_mainParam(IntervalInt(0, 0)),
      m_mainParamDefault(true)
    {
    }

    [[nodiscard]] bool check(
        Modulation mod, int32_t speed, int32_t mainParam) const
    {
        bool modulation = m_modDefault || (m_mod == mod);
        if (modulation)
        {
            bool sp = m_speed.inInterval(speed) || m_speedDefault;
            bool main = m_mainParam.inInterval(mainParam) || m_mainParamDefault;
            return sp && main;
        }
        return false;
    }

    [[nodiscard]] bool check(Modulation mod) const
    {
        bool modulation = m_modDefault || (m_mod == mod);
        return modulation && m_speedDefault && m_mainParamDefault;
    }

    void clear()
    {
        m_mod = Modulation::Unrecognize;
        m_speed = IntervalInt(0, 0);
        m_mainParam = IntervalInt(0, 0);
        m_modDefault = true;
        m_speedDefault = true;
        m_mainParamDefault = true;
    }

private:
    Modulation m_mod;
    bool m_modDefault = false;
    IntervalInt m_speed;
    bool m_speedDefault = false;
    IntervalInt m_mainParam;
    bool m_mainParamDefault = false;
};

class DroneSquare
{
public:
    explicit DroneSquare() = default;
    DroneSquare(
        const IntervalInt& lengthInMkC,
        const IntervalInt& wideInkHz,
        ModulationChecker mod) :
      m_lengthMKC(lengthInMkC),
      m_wideInKHz(wideInkHz),
      m_modulation(std::move(mod))
    {
    }

    [[nodiscard]] IntervalInt lengthMKC() const
    {
        return m_lengthMKC;
    }
    void setLengthMKC(const IntervalInt& lengthMKC)
    {
        m_lengthMKC = lengthMKC;
    }
    [[nodiscard]] IntervalInt wideInKHz() const
    {
        return m_wideInKHz;
    }
    void setWideInKHz(const IntervalInt& wideKHz)
    {
        m_wideInKHz = wideKHz;
    }
    [[nodiscard]] std::vector<float> akf() const
    {
        return m_akf;
    }

    [[nodiscard]] ModulationChecker modulation() const
    {
        return m_modulation;
    }

    void setModulation(Modulation mod)
    {
        m_modulation = ModulationChecker(mod);
    }
    void clear()
    {
        m_lengthMKC.clear();
        m_wideInKHz.clear();
        m_akf.clear();
        m_modulation.clear();
    }

private:
    IntervalInt m_lengthMKC;
    IntervalInt m_wideInKHz;
    std::vector<float> m_akf;
    ModulationChecker m_modulation;
};

class DroneType
{
public:
    DroneType(
        const IntervalInt& freqInKHz,
        int32_t minQuantSquares,
        int32_t identificationNumber,
        std::string droneName) :
      m_droneName(std::move(droneName)),
      m_freqInKHz(freqInKHz),
      m_minQuantSquares(minQuantSquares),
      m_identificationNumber(identificationNumber)
    {
    }
    DroneType() = default;

    void clear()
    {
        m_ds.clear();
        m_freqInKHz.clear();
        m_identificationNumber = 0;
        m_minQuantSquares = 0;
        m_droneName = nullptr;
    }
    [[nodiscard]] std::string droneName() const
    {
        return m_droneName;
    }
    [[nodiscard]] IntervalInt freqInKHz() const // range of detecting
    {
        return m_freqInKHz;
    }
    [[nodiscard]] int32_t minQuantSquares() const
    {
        return m_minQuantSquares;
    }
    [[nodiscard]] int32_t identificationNumber() const
    {
        return m_identificationNumber;
    }
    [[nodiscard]] const std::vector<DroneSquare>& ds() const // description
                                                             // simple block
    {
        return m_ds;
    }
    void addDescription(const DroneSquare& ds)
    {
        m_ds.push_back(ds);
    }

private:
    std::string m_droneName;
    IntervalInt m_freqInKHz;
    int32_t m_minQuantSquares = 0;
    int32_t m_identificationNumber = 0;
    std::vector<DroneSquare> m_ds;
};

class DroneResult
{
public:
    DroneResult() = default;
    DroneResult(int localNumber, float probability) :
      m_localNumber(localNumber), m_probability(probability)
    {
    }

    bool compare(DroneResult& s) const
    {
        return localNumber() == s.localNumber();
    }

    [[nodiscard]] int32_t localNumber() const
    {
        return m_localNumber;
    }
    [[nodiscard]] float probability() const // number of detected data
    {                                       // blocks
        return m_probability;
    }
    [[nodiscard]] int32_t tag() const // for different use
    {
        return m_tag;
    }
    void setTag(int32_t tag)
    {
        m_tag = tag;
    }
    void update(float probability, int32_t tag)
    {
        m_probability = probability;
        m_tag = tag;
    }
    void clear()
    {
        m_localNumber = 0;
        m_probability = 0;
        m_tag = 0;
    }

private:
    int m_localNumber = 0;
    float m_probability = 0;
    int32_t m_tag = 0;
};

}; // namespace mpk::signal_scanner::detector