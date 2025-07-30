#pragma once

#include "core/Frequency.h"
#include "core/Geo.h"

#include "core/device/ChannelStatus.h"
#include "core/device/TransmissionData.h"

#include <QObject>

#include <vector>

namespace mpk::drone::core
{

struct ChannelZone
{
    Angle azimuth;
    Angle span;
    Distance distance;
};

inline bool operator==(const ChannelZone& left, const ChannelZone& right);
inline bool operator!=(const ChannelZone& left, const ChannelZone& right);

struct ChannelParams
{
    Frequency sampleRate;
    Frequency bandwidth;
};

inline bool operator==(const ChannelParams& left, const ChannelParams& right);
inline bool operator!=(const ChannelParams& left, const ChannelParams& right);

struct ChannelData
{
    ChannelStatus status;
    ChannelZone zone;
    ChannelParams params;
    std::vector<TransmissionData> transmissions;
};

class ChannelDataHolder : public QObject
{
    Q_OBJECT

public:
    explicit ChannelDataHolder(
        std::vector<ChannelData> channels, QObject* parent = nullptr);

    [[nodiscard]] int count() const;

    void setStatus(int index, ChannelStatus status);
    [[nodiscard]] ChannelStatus status(int index) const;

    void setZone(int index, const ChannelZone& zone);
    [[nodiscard]] ChannelZone zone(int index) const;

    void setParams(int index, const ChannelParams& params);
    [[nodiscard]] ChannelParams params(int index) const;

    void setTransmissions(
        int index, const std::vector<TransmissionData>& transmissions);
    [[nodiscard]] std::vector<TransmissionData> transmissions(int index) const;

private:
    void checkIndex(int index) const;

signals:
    void statusChanged(int, ChannelStatus);
    void zoneChanged(int, ChannelZone);
    void paramsChanged(int, ChannelParams);
    void transmissionsChanged(int, std::vector<TransmissionData>);

private:
    std::vector<ChannelData> m_channels;
};

} // namespace mpk::drone::core

Q_DECLARE_METATYPE(mpk::drone::core::ChannelZone);
Q_DECLARE_METATYPE(mpk::drone::core::ChannelParams);
