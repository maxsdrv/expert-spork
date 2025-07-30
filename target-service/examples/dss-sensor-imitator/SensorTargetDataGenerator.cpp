#include "SensorTargetDataGenerator.h"

#include "dss-backend/rest/SensorTargetData.h"
#include "geolocation/GeolocationController.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

namespace mpk::dss
{

namespace
{

const auto descriptionList = std::vector<std::string>{
    "DJI Mavic 3",
    "DJI Mavic Air 2",
    "DJI Mini 2",
    "Autel Robotics Evo",
    "Potensic Atom SE",
    "Skydio 2+"};

const auto frequenciesList = std::vector<backend::rest::Frequency>{
    1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999};

} // namespace

SensorTargetDataGenerator::SensorTargetDataGenerator(
    gsl::not_null<const GeolocationController*> geolocationController,
    std::vector<backend::rest::SingleChannelInfo> channels,
    bool hasTargetPosition) :
  m_geolocationController(geolocationController),
  m_channels(std::move(channels)),
  m_hasTargetPosition(hasTargetPosition),
  m_descriptionDist(0, static_cast<int>(descriptionList.size() - 1)),
  m_frequenciesDist(0, static_cast<int>(frequenciesList.size() - 1)),
  // NOLINTBEGIN
  // (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  m_frequenciesCountDist(5, 20),
  // NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  m_generator(m_randomDevice())
{
}

backend::rest::SensorTargetData SensorTargetDataGenerator::generate()
{
    auto channelDistribution = std::uniform_int_distribution(
        0, static_cast<int>(m_channels.size()) - 1);
    auto channel = channelDistribution(m_generator);

    auto bearing = std::optional<double>{};
    auto distance = std::optional<double>{};
    auto targetLocation = std::optional<QGeoCoordinate>{};

    if (m_hasTargetPosition)
    {
        auto imitatorGeolocation = m_geolocationController->current();
        auto bearingDistribution = std::uniform_real_distribution(
            imitatorGeolocation.azimuth + m_channels.at(channel).viewAngle.min,
            imitatorGeolocation.azimuth + m_channels.at(channel).viewAngle.max);
        bearing = bearingDistribution(m_generator);
        auto distanceDistribution =
            std::uniform_real_distribution(0., m_channels.at(channel).range);
        distance = distanceDistribution(m_generator);
        targetLocation = imitatorGeolocation.location.atDistanceAndAzimuth(
            *distance, *bearing);
    }

    return {
        .type = backend::rest::SensorTargetData::defaultType,
        .id = m_count++,
        .channel = channel,
        .description = descriptionList.at(m_descriptionDist(m_generator)),
        .bearing = bearing,
        .distance = distance,
        .coordinate = targetLocation,
        .frequencies = generateFrequencies(),
        .blocksCount = 0,
        .timestamp = QDateTime::currentDateTime()};
}

std::vector<backend::rest::Frequency> SensorTargetDataGenerator::
    generateFrequencies() const
{
    auto frequencies = std::set<backend::rest::Frequency>{};
    std::generate_n(
        std::inserter(frequencies, frequencies.begin()),
        m_frequenciesCountDist(m_generator),
        [this]()
        { return frequenciesList.at(m_frequenciesDist(m_generator)); });
    return std::vector<backend::rest::Frequency>{
        frequencies.begin(), frequencies.end()};
}

} // namespace mpk::dss
