#pragma once

#include "core/Frequency.h"

#include <QDateTime>
#include <QObject>

#include <cstdint>
#include <string>

namespace mpk::drone::core
{

using TransmissionId = int32_t;

struct TransmissionData
{
    int number = 0;
    TransmissionId id = 0;
    std::string name;
    QDateTime timestamp;
    FrequencyRange frequencyRange;
    int blocksCount = 0;
    int minBlocksCount = 0;
    double detectionProbability = 0.;
};

bool operator==(const TransmissionData& left, const TransmissionData& right);
bool operator!=(const TransmissionData& left, const TransmissionData& right);

} // namespace mpk::drone::core

Q_DECLARE_METATYPE(mpk::drone::core::TransmissionData);
