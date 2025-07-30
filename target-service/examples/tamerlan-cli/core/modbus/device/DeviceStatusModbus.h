#pragma once

#include "core/device/DeviceStatus.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

class ModbusData;

void connectDeviceStatusWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<DeviceStatusHolder*> holder);

} // namespace mpk::drone::core
