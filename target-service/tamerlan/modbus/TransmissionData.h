#pragma once

#include "Common.h"

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

#include "gsl/assert"

#include <array>
#include <tuple>

namespace mpk::dss::backend::tamerlan
{

#pragma pack(push, 2)

struct TransmissionData
{
    uint32_t id = 0;
    uint32_t handbookId = 0;
    static constexpr auto nameMaxSize = 64;
    std::array<char, nameMaxSize> name = {};
    int64_t timestamp = 0;
    int32_t detectionLevel = 0;
    int32_t detectionThreshold = 0;
    int32_t beginFreq = 0;
    int32_t endFreq = 0;
    float detectionProbability = 0.0F;
    static constexpr auto reserveSize = 28;
    std::array<char, reserveSize> reserve = {};

    static void toRegs(
        const TransmissionData& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        it = uint32ToRegs(data.id, it);
        it = uint32ToRegs(data.handbookId, it);
        it = charArrayToRegs(data.name.data(), data.name.size(), it);
        it = int64ToRegs(data.timestamp, it);
        it = int32ToRegs(data.detectionLevel, it);
        it = int32ToRegs(data.detectionThreshold, it);
        it = int32ToRegs(data.beginFreq, it);
        it = int32ToRegs(data.endFreq, it);
        it = floatToRegs(data.detectionProbability, it);
        for (int i = 0; i < reserveSize / 2; i++)
        {
            *it = 0;
        }
    }

    static TransmissionData fromRegs(
        U16Iterator auto begin, U16Iterator auto end)
    {
        Expects(std::distance(begin, end) == sizeof(TransmissionData) / 2);

        TransmissionData result;
        auto it = begin;
        std::tie(result.id, it) = regsToUint32(it);
        std::tie(result.handbookId, it) = regsToUint32(it);
        it = regsToCharArray(result.name.data(), result.name.size(), it);
        std::tie(result.timestamp, it) = regsToInt64(it);
        std::tie(result.detectionLevel, it) = regsToInt32(it);
        std::tie(result.detectionThreshold, it) = regsToInt32(it);
        std::tie(result.beginFreq, it) = regsToInt32(it);
        std::tie(result.endFreq, it) = regsToInt32(it);
        std::tie(result.detectionProbability, it) = regsToFloat(it);
        return result;
    }
};

#pragma pack(pop)

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(
    sizeof(TransmissionData) == 128,
    "Size of TransmissionDataModbus is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::dss::backend::tamerlan

template <>
struct ToJsonConverter<mpk::dss::backend::tamerlan::TransmissionData>
{
    static QJsonValue convert(
        const mpk::dss::backend::tamerlan::TransmissionData& data);
};

template <>
struct FromJsonConverter<mpk::dss::backend::tamerlan::TransmissionData>
{
    static mpk::dss::backend::tamerlan::TransmissionData get(
        const QJsonValue& value);
};
