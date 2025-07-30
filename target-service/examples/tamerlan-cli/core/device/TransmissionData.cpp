#include "TransmissionData.h"

namespace mpk::drone::core
{

bool operator==(const TransmissionData& left, const TransmissionData& right)
{
    return std::tie(
               left.blocksCount,
               left.minBlocksCount,
               left.detectionProbability,
               left.frequencyRange,
               left.id,
               left.number,
               left.name,
               left.timestamp)
           == std::tie(
               right.blocksCount,
               right.minBlocksCount,
               right.detectionProbability,
               right.frequencyRange,
               right.id,
               right.number,
               right.name,
               right.timestamp);
}

bool operator!=(const TransmissionData& left, const TransmissionData& right)

{
    return !(left == right);
}

} // namespace mpk::drone::core
