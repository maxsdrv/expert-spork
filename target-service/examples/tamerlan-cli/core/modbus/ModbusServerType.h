#pragma once

#include <QObject>

namespace mpk::drone::core
{

enum class ModbusServerType
{
    TCP = 0,
    SERIAL
};

ModbusServerType modbusServerTypeFromString(const std::string& str);
std::string modbusServerTypeToString(ModbusServerType value);

} // namespace mpk::drone::core
