#pragma once

#include "dss-backend/tamerlan/modbus/ModbusServerAddress.h"

#include <cinttypes>
#include <variant>

namespace mpk::dss::backend::tamerlan
{

struct Command
{
    enum Type
    {
        READ_INPUT_REGISTERS,
        READ_HOLDING_REGISTERS,
        READ_COILS,
        WRITE_HOLDING_REGISTERS,
        WRITE_COILS
    };

    Type type;
    int address;
    int numRegs;
    std::variant<uint16_t*, uint8_t*> data;
    ModbusServerAddress serverAddress;
};

bool operator==(const Command& l, const Command& r);

} // namespace mpk::dss::backend::tamerlan
