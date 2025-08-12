#include "GeoData.h"

namespace mpk::drone::core
{

bool operator==(const GeoData& left, const GeoData& right)
{
    return std::tie(left.azimuth, left.position)
           == std::tie(right.azimuth, right.position);
}

bool operator!=(const GeoData& left, const GeoData& right)
{
    return !(left == right);
}

GeoDataHolder::GeoDataHolder(const GeoData& data, QObject* parent) :
  QObject(parent), m_data(data)
{
}

void GeoDataHolder::setData(const GeoData& data)
{
    if (m_data != data)
    {
        m_data = data;
        emit changed(m_data);
    }
}

GeoData GeoDataHolder::data() const
{
    return m_data;
}

void GeoDataHolder::setPosition(const Position& position)
{
    if (m_data.position != position)
    {
        m_data.position = position;
        emit changed(m_data);
    }
}

Position GeoDataHolder::position() const
{
    return m_data.position;
}

void GeoDataHolder::setAzimuth(Angle azimuth)
{
    if (m_data.azimuth != azimuth)
    {
        m_data.azimuth = azimuth;
        emit changed(m_data);
    }
}

Angle GeoDataHolder::azimuth() const
{
    return m_data.azimuth;
}

} // namespace mpk::drone::core
