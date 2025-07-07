#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct AlarmLevel
{
    Q_GADGET;

public:
    enum Value
    {
        UNDEFINED = -1,
        NONE,
        MEDIUM,
        HIGH,
        CRITICAL
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value state);
};

} // namespace mpk::dss::core
