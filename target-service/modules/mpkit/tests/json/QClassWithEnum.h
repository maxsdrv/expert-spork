#pragma once

#include <QObject>

namespace json::tests
{
class QClassWithEnum : public QObject
{
    Q_OBJECT
public:
    enum class Enumeration
    {
        ONE = 1,
        TWO = 2
    };

    Q_ENUM(Enumeration);
};

}
