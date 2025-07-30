#pragma once

/**
 * Main class of drone scanner
 * We suppose normal work with sampleRate between 10e6 .. 60e6
 */

#ifdef __cplusplus

#include <cstddef>

extern "C"
{

#else

#include "stddef.h"

#endif

    // create object
    void* detectorDummyInit();
    // return 1 if params is good
    int detectorDummySetParam(
        void* object, int sampleRateKHz, int bandInkHz, int centralFreqKHz);

    // result = 1 when internal buffer is full or 0     samples in complexFloat
    // we suppose float interval between -20000 ... 20000
    int detectorDummyRun(
        void* object, int centralFreqKHz, float* complexData, int samples);

    // result = 1 when internal buffer is full or 0     samples in complexInt16
    int detectorDummyRunInt16(
        void* object, int centralFreqKHz, void* complexData, int samples);

    // result is ready
    int detectorResultReady(void* object);

    // wait result - current thread wait
    int detectorWaitResult(void* object);

    void writeToFile(
        const char* path, const float* complexData, int samples, int freq);

    void readFromFile(
        const char* path, float* complexData, size_t* samples, size_t* pointer);

    // get quant of finding drones
    int detectorDummyDronesQuant(void* object);
    // request a specific drone name by order number
    const char* detectorDummyDronesName(void* object, int namePosition);

    void detectorDummyDelete(void* object);

    int interferenceGenerationBufferSize(void* object);

    void interferenceGeneration(
        void* object,
        char* name,
        int* centerFreqKHz,
        float* complexData,
        int samples);
    void interferenceGenerationReset(void* object);

#ifdef __cplusplus
}

#endif