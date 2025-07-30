#pragma once

// clang-format off
// NOLINTBEGIN

#include <QtEndian>
#include "enums.h"

///
/// \brief toByteOrderValue
/// \param value
/// \param order
/// \return
///
inline quint16 toByteOrderValue(quint16 value, ByteOrder order)
{
    switch(order)
    {
        case ByteOrder::BigEndian:
            return qToBigEndian<quint16>(value);

        case ByteOrder::LittleEndian:
            return qToLittleEndian<quint16>(value);
    }

    return value;
}

// NOLINTEND
// clang-format on
