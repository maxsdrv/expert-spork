#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct DeviceState
{
    Q_GADGET;

public:
    enum Value
    {
        UNAVAILABLE,
        OFF,
        FAILURE,
        OK
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value state);
};

} // namespace mpk::dss::core
