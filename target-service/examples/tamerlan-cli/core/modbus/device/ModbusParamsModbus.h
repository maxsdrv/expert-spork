#pragma once

#include "core/device/ModbusParams.h"

#include "gsl/pointers"

namespace mpk::drone::core
{

class ModbusData;

void connectModbusParamsWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<ModbusParamsHolder*> holder);

} // namespace mpk::drone::core
