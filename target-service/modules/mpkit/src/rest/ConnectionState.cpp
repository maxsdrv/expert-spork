#include "mpk/rest/ConnectionState.h"

#include "qt/MetaEnum.h"

namespace mpk::rest
{

ConnectionState::ConnectionState(QObject* parent) : QObject(parent)
{
}

ConnectionState::Value ConnectionState::fromQString(const QString& string)
{
    return qt::metaEnumFromString<Value>(string);
}

QString ConnectionState::toQString(const Value& value)
{
    return qt::metaEnumToString(value);
}

QString ConnectionState::toDisplayString(Value value)
{
    switch (value)
    {
        case Value::DISCONNECTED: return tr("Disconnected");
        case Value::CONNECTING: return tr("Connecting");
        case Value::CONNECTED: return tr("Connected");
        default: return tr("Invalid value");
    }
}

} // namespace mpk::rest
