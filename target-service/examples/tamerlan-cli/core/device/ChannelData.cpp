#include "ChannelData.h"

#include "gsl/assert"

namespace mpk::drone::core
{

bool operator==(const ChannelZone& left, const ChannelZone& right)
{
    return std::tie(left.azimuth, left.distance, left.span)
           == std::tie(right.azimuth, right.distance, right.span);
}

bool operator!=(const ChannelZone& left, const ChannelZone& right)
{
    return !(left == right);
}

bool operator==(const ChannelParams& left, const ChannelParams& right)
{
    return std::tie(left.bandwidth, left.sampleRate)
           == std::tie(right.bandwidth, right.sampleRate);
}

bool operator!=(const ChannelParams& left, const ChannelParams& right)
{
    return !(left == right);
}

ChannelDataHolder::ChannelDataHolder(
    std::vector<ChannelData> channels, QObject* parent) :
  QObject(parent), m_channels(std::move(channels))
{
}

int ChannelDataHolder::count() const
{
    return static_cast<int>(m_channels.size());
}

void ChannelDataHolder::setStatus(int index, ChannelStatus status)
{
    checkIndex(index);
    if (m_channels.at(index).status != status)
    {
        m_channels.at(index).status = status;
        emit statusChanged(index, status);
    }
}

ChannelStatus ChannelDataHolder::status(int index) const
{
    checkIndex(index);
    return m_channels.at(index).status;
}

void ChannelDataHolder::setZone(int index, const ChannelZone& zone)
{
    checkIndex(index);
    if (m_channels.at(index).zone != zone)
    {
        m_channels.at(index).zone = zone;
        emit zoneChanged(index, zone);
    }
}

ChannelZone ChannelDataHolder::zone(int index) const
{
    checkIndex(index);
    return m_channels.at(index).zone;
}

void ChannelDataHolder::setParams(int index, const ChannelParams& params)
{
    checkIndex(index);
    if (m_channels.at(index).params != params)
    {
        m_channels.at(index).params = params;
        emit paramsChanged(index, params);
    }
}

ChannelParams ChannelDataHolder::params(int index) const
{
    checkIndex(index);
    return m_channels.at(index).params;
}

void ChannelDataHolder::setTransmissions(
    int index, const std::vector<TransmissionData>& transmissions)
{
    checkIndex(index);
    if (m_channels.at(index).transmissions != transmissions)
    {
        m_channels.at(index).transmissions = transmissions;
        emit transmissionsChanged(index, transmissions);
    }
}

std::vector<TransmissionData> ChannelDataHolder::transmissions(int index) const
{
    checkIndex(index);
    return m_channels.at(index).transmissions;
}

void ChannelDataHolder::checkIndex(int index) const
{
    Expects(index >= 0);
    Expects(index < static_cast<int>(m_channels.size()));
}

} // namespace mpk::drone::core
