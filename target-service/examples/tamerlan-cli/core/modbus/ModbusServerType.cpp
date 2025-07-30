#include "ModbusServerType.h"

#include "exception/General.h"
#include "exception/InvalidValue.h"

#include <map>

namespace mpk::drone::core
{

std::map<std::string, ModbusServerType> dictionary()
{
    return {
        {"tcp", ModbusServerType::TCP}, {"serial", ModbusServerType::SERIAL}};
}

ModbusServerType modbusServerTypeFromString(const std::string& str)
{
    auto dict = dictionary();
    auto it = dict.find(str);
    if (it == dict.end())
    {
        BOOST_THROW_EXCEPTION(
            exception::InvalidValue()
            << exception::ExceptionInfo(std::string("Unknown string: ") + str));
    }
    return it->second;
}

std::string modbusServerTypeToString(ModbusServerType value)
{
    auto dict = dictionary();
    auto it = std::find_if(
        dict.begin(),
        dict.end(),
        [value](const auto& record) { return record.second == value; });
    if (it == dict.end())
    {
        BOOST_THROW_EXCEPTION(
            exception::InvalidValue() << exception::ExceptionInfo(
                std::string("String is not defined for value: ")
                + std::to_string(static_cast<int>(value))));
    }
    return it->first;
}

} // namespace mpk::drone::core
