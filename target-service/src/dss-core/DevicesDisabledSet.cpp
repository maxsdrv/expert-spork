
#include "dss-core/DevicesDisabledSet.h"
#include "dss-core/SensorNotifier.h"

#include "log/Log2.h"

namespace mpk::dss::core
{

void setDevicesDisabled(
    const DisabledStateList& dList,
    std::shared_ptr<SensorProviderPtrs>& sensorProviders,
    std::shared_ptr<JammerProviderPtrs>& jammerProviders)
{
    LOG_TRACE << "setDevicesDisabled(): start";
    for (const auto& device: dList)
    {
        LOG_TRACE << "setDevicesDisabled(): " << device.getId() << "  "
                  << device.isDisabled();

        QString id = device.getId();
        if (auto* sensor =
                findDevice<SensorProperties>(sensorProviders, DeviceId(id)))
        {
            sensor->properties().setDisabled(device.isDisabled());
        }
        else if (
            auto* jammer =
                findDevice<JammerProperties>(jammerProviders, DeviceId(id)))
        {
            jammer->properties().setDisabled(device.isDisabled());
        }
        else
        {
            LOG_WARNING << "setDevicesDisabled(): Device id not found: "
                        << static_cast<QString>(id).toStdString();
        }
    }
}

template <class Device>
SettingsDevice settingsDevice(const Device& it)
{
    SettingsDevice setsDevice;
    setsDevice.setId(static_cast<QString>(it.id()));
    setsDevice.setDisabled(it.properties().disabled());

    auto pos = it.properties().position();
    OAService::OAServiceGeo_position oaPosition;

    OAService::OAServiceGeo_coordinate oaCoord;
    oaCoord.setAltitude(pos.coordinate.altitude());
    oaCoord.setLatitude(pos.coordinate.latitude());
    oaCoord.setLongitude(pos.coordinate.longitude());

    oaPosition.setAzimuth(pos.azimuth);
    oaPosition.setCoordinate(oaCoord);
    setsDevice.setPosition(oaPosition);
    OAService::OAServiceGeo_position_mode oaMode;
    oaMode.setValue(it.properties().positionMode());
    setsDevice.setPositionMode(oaMode);

    return setsDevice;
}

DisabledStateList getDevicesSettings(
    const std::shared_ptr<SensorProviderPtrs>& sensorProviders,
    const std::shared_ptr<JammerProviderPtrs>& jammerProviders)
{
    DisabledStateList list;
    if (jammerProviders)
    {
        for (const auto& provider: *jammerProviders)
        {
            for (const auto& it: *provider)
            {
                list.append(settingsDevice(it));
            }
        }
    }
    if (sensorProviders)
    {
        for (const auto& provider: *sensorProviders)
        {
            for (const auto& it: *provider)
            {
                list.append(settingsDevice(it));
            }
        }
    }
    return list;
}

} // namespace mpk::dss::core
