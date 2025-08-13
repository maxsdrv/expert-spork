#pragma once

#include <QObject>

namespace mpk::dss::core
{

struct JammerMode
{
    Q_GADGET;

public:
    enum Value
    {
        UNDEFINED = -1,
        AUTO,
        MANUAL
    };
    Q_ENUM(Value);

    static Value fromString(const QString& string);
    static QString toString(Value mode);
};

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::JammerMode::Value);
