#pragma once
#include <complex>
#include <cstring>
#include <memory>
#include <span>
#include <vector>

#include "CommonTypes.hpp"
#include "DroneType.hpp"
#include "ResultsStruct.hpp"

namespace mpk::signal_scanner::detector
{

/**
 * Main class of drone scanner
 * We suppose normal work with sampleRate between 10e6 .. 60e6
 *
 * class classified type of some drones and generate interference
 */

class DetectorEngine;

class Detector
{

public:
    /**
     * initDescription - formal description for communication channels
     * bufferSizeToRunInMkC - the minimum duration in microseconds for the
     * analysis
     * if initDescription == nullptr module use default constructor
     */
    explicit Detector(
        const std::vector<DroneType>& initDescription,
        TimeMks bufferSizeToRun); // 0.1 seconds

    virtual ~Detector();

    std::vector<DroneType> description();

    bool setParam(
        FrequencyKHz sampleRate, FrequencyKHz bandIn, FrequencyKHz centralFreq);

    [[nodiscard]] bool run(
        FrequencyKHz centralFreq, const std::span<t16sc>& data);
    [[nodiscard]] bool run(
        FrequencyKHz centralFreq, const std::span<t32fc>& data);

    // The size of the data block after which the result is returned
    [[nodiscard]] int32_t blockSize() const;

    // name of finded drones type
    std::vector<DroneResult> currentDrones();

    /** Generating interference to suppress the drone
     *Call after successfully finding drones (not for all types of drones)
     */
    void resetInterferenceGeneration();
    [[nodiscard]] const float* interferenceGeneration(
        size_t& size, FrequencyKHz& centerFreq);

private:
    std::unique_ptr<DetectorEngine> m_engine;
};

} // namespace mpk::signal_scanner::detector
