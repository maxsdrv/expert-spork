#include "SensorWorkzoneVsChannelInfo.h"

#include "dss-core/DeviceWorkzoneSector.h"

#include <algorithm>

namespace mpk::dss::backend::rest
{

using core::DeviceWorkzone;
using core::DeviceWorkzoneSector;

DeviceWorkzone DeviceWorkzoneVsChannelInfo::convert(
    const ChannelInfo& channelInfo)
{
    DeviceWorkzone sensorWorkzone;

    auto count = 0;
    std::transform(
        channelInfo.objects.begin(),
        channelInfo.objects.end(),
        std::back_inserter(sensorWorkzone.sectors),
        [&count](const auto& singleChannelInfo) -> DeviceWorkzoneSector
        {
            return {
                .number = count++,
                .distance = singleChannelInfo.range,
                .minAngle = singleChannelInfo.viewAngle.min,
                .maxAngle = singleChannelInfo.viewAngle.max};
        });

    return sensorWorkzone;
}

} // namespace mpk::dss::backend::rest
