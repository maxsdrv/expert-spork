#pragma once

#include "mpk/stable_link/State.h"

#include <QObject>

namespace mpk::stable_link
{

class StateQt : public QObject
{
    Q_OBJECT

public:
    explicit StateQt(QObject* parent = nullptr);

    enum class Value : std::uint8_t
    {
        OFFLINE = static_cast<int>(State::OFFLINE),
        DISCONNECTED = static_cast<int>(State::DISCONNECTED),
        CONNECTING = static_cast<int>(State::CONNECTING),
        CONNECTED = static_cast<int>(State::CONNECTED)
    };
    Q_ENUM(Value)

    Q_INVOKABLE static mpk::stable_link::StateQt::Value fromQString(const QString& string);
    Q_INVOKABLE static QString toQString(mpk::stable_link::StateQt::Value value);
    Q_INVOKABLE static QString toDisplayString(mpk::stable_link::StateQt::Value value);
};

} // namespace mpk::stable_link
