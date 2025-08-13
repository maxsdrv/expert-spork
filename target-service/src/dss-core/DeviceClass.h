#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct DeviceClass
{
    Q_GADGET;

public:
    enum Value
    {
        SENSOR,
        JAMMER
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value state);
};

} // namespace mpk::dss::core
