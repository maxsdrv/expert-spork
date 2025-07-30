#pragma once

#include <QObject>

namespace mpk::rest
{

// TODO: A duplicate of the rctcore::ConnectionState,
// take it to the common library.
class ConnectionState : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionState(QObject* parent = nullptr);

    enum Value
    {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    };
    Q_ENUM(Value)

    Q_INVOKABLE static ConnectionState::Value fromQString(const QString& string);
    Q_INVOKABLE static QString toQString(const ConnectionState::Value& value);
    Q_INVOKABLE static QString toDisplayString(ConnectionState::Value value);
};

} // namespace mpk::rest
