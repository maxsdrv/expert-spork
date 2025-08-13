#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct DeviceType
{
    Q_GADGET;

public:
    enum Value
    {
        UNDEFINED = -1,
        RFD,
        RADAR,
        CAMERA
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value state);
};

} // namespace mpk::dss::core
