#include "TransmissionData.h"

using namespace mpk::dss::backend::tamerlan;

constexpr auto idTag = "id";
constexpr auto handbookIdTag = "handbook_id";
constexpr auto nameTag = "name";
constexpr auto timestampTag = "timestamp";
constexpr auto blocksCountTag = "blocks_count";
constexpr auto minQuantSquaresTag = "min_quant_squares";
constexpr auto beginFreqTag = "begin_freq";
constexpr auto endFreqTag = "end_freq";
constexpr auto detectionProbabilityTag = "detection_probability";

QJsonValue ToJsonConverter<TransmissionData>::convert(
    const TransmissionData& data)
{
    return QJsonObject{
        {idTag, json::toValue(data.id)},
        {handbookIdTag, json::toValue(data.handbookId)},
        {nameTag, json::toValue(QString::fromUtf8(data.name.data()))},
        {timestampTag, json::toValue(data.timestamp)},
        {blocksCountTag, json::toValue(data.blocksCount)},
        {minQuantSquaresTag, json::toValue(data.minQuantSquares)},
        {beginFreqTag, json::toValue(data.beginFreq)},
        {endFreqTag, json::toValue(data.endFreq)},
        {detectionProbabilityTag, json::toValue(data.detectionProbability)}};
}

TransmissionData FromJsonConverter<TransmissionData>::get(
    const QJsonValue& value)
{
    auto object = json::fromValue<QJsonObject>(value);
    std::array<char, TransmissionData::nameMaxSize> name = {};
    auto nameStr = json::fromObject<QString>(object, nameTag);
    for (int i = 0; i < nameStr.length(); i++)
    {
        name.at(i) = nameStr.at(i).toLatin1();
    }
    return TransmissionData{
        .id = json::fromObject<uint32_t>(object, idTag),
        .handbookId = json::fromObject<uint32_t>(object, handbookIdTag),
        .name = name,
        .timestamp = json::fromObject<int64_t>(object, timestampTag),
        .blocksCount = json::fromObject<int32_t>(object, blocksCountTag),
        .minQuantSquares =
            json::fromObject<int32_t>(object, minQuantSquaresTag),
        .beginFreq = json::fromObject<int32_t>(object, beginFreqTag),
        .endFreq = json::fromObject<int32_t>(object, endFreqTag),
        .detectionProbability =
            json::fromObject<float>(object, detectionProbabilityTag)};
}
