#pragma once

// clang-format off
// NOLINTBEGIN

#include <QVariant>
#include "enums.h"

///
/// \brief The ModbusWriteParams class
///
struct ModbusWriteParams
{
    quint32 Node;
    quint32 Address;
    QVariant Value;
    DataDisplayMode DisplayMode;
    ByteOrder Order;
};

///
/// \brief The ModbusMaskWriteParams class
///
struct ModbusMaskWriteParams
{
    quint32 Node;
    quint32 Address;
    quint16 AndMask;
    quint16 OrMask;
};

// NOLINTEND
// clang-format on
