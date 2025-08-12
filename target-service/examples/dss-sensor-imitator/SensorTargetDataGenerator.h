#pragma once

#include "dss-backend/rest/SensorTargetData.h"
#include "dss-backend/rest/channel/SingleChannelInfo.h"

#include "gsl/pointers"

#include <random>

namespace mpk::dss
{

class GeolocationController;

class SensorTargetDataGenerator
{
public:
    SensorTargetDataGenerator(
        gsl::not_null<const GeolocationController*> geolocationController,
        std::vector<backend::rest::SingleChannelInfo> channels,
        bool hasTargetPosition);

    backend::rest::SensorTargetData generate();

private:
    std::vector<backend::rest::Frequency> generateFrequencies() const;

private:
    gsl::not_null<const GeolocationController*> m_geolocationController;
    std::vector<backend::rest::SingleChannelInfo> m_channels;
    bool m_hasTargetPosition;

    int m_count = 0;
    mutable std::uniform_int_distribution<> m_descriptionDist;
    mutable std::uniform_int_distribution<> m_frequenciesDist;
    mutable std::uniform_int_distribution<> m_frequenciesCountDist;
    std::random_device m_randomDevice;
    mutable std::mt19937 m_generator;
};

} // namespace mpk::dss
