#pragma once

#include "dss-core/JammerDevice.h"
#include "dss-core/SensorDevice.h"

#include "OAServiceDevice_settings.h"

namespace mpk::dss::core
{

using SettingsDevice = OAService::OAServiceDevice_settings;
using DisabledStateList = QList<SettingsDevice>;

template <class Properties>
void setDeviceDisabledState(
    Device<Properties>& dev, const DisabledStateList* list)
{
    auto it = std::find_if(
        list->cbegin(),
        list->cend(),
        [&dev](const SettingsDevice& setsDev)
        { return setsDev.getId() == static_cast<QString>(dev.id()); });
    if (it != list->end())
    {
        bool disabled = (*it).isDisabled();
        LOG_DEBUG << "Setting Registered device: " << dev.id()
                  << " disabled state: " << disabled;
        dev.properties().setDisabled(disabled);
    }
}

template <class Properties>
void setDevicePosition(
    Device<Properties>& dev,
    const std::vector<DevicePositionExtended>* positions)
{
    auto it = std::find_if(
        positions->cbegin(),
        positions->cend(),
        [&dev](const DevicePositionExtended& dbPos)
        { return dbPos.key == static_cast<QString>(dev.id()); });
    if (it != positions->end())
    {
        auto pos = (*it).position;
        LOG_DEBUG << "Setting Registered device: " << dev.id()
                  << " azimuth=" << pos.azimuth
                  << " coordinate=" << pos.coordinate.toString();
        dev.properties().setPosition(pos);
        dev.properties().setPositionMode((*it).mode);
    }
}

void setDevicesDisabled(
    const DisabledStateList& dList,
    std::shared_ptr<SensorProviderPtrs>& sensorProviders,
    std::shared_ptr<JammerProviderPtrs>& jammerProviders);

DisabledStateList getDevicesSettings(
    const std::shared_ptr<SensorProviderPtrs>& sensorProviders,
    const std::shared_ptr<JammerProviderPtrs>& jammerProviders = nullptr);

} // namespace mpk::dss::core
