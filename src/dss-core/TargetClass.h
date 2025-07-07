#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct TargetClass
{
    Q_GADGET

public:
    enum Value
    {
        UNDEFINED = -1,
        DRONE,
        CAR,
        TRUCK,
        HUMAN,
        TREE,
        PEOPLE_GROUP,
        MOTORCYCLE,
        JET_SKI,
        BOAT,
        SHIP,
        REMOTE_CONSOLE
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value targetClass);
};

} // namespace mpk::dss::core
