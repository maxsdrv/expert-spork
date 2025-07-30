#include "mpk/stable_link/StateQt.h"

#include "qt/MetaEnum.h"

namespace mpk::stable_link
{

StateQt::StateQt(QObject* parent) : QObject(parent)
{
}

StateQt::Value StateQt::fromQString(const QString& string)
{
    return qt::metaEnumFromString<Value>(string);
}

QString StateQt::toQString(Value value)
{
    return qt::metaEnumToString(value);
}

QString StateQt::toDisplayString(Value value)
{
    switch (value)
    {
        case Value::OFFLINE: return tr("Offline");
        case Value::DISCONNECTED: return tr("Disconnected");
        case Value::CONNECTING: return tr("Connecting");
        case Value::CONNECTED: return tr("Connected");

        default:
            BOOST_THROW_EXCEPTION(std::invalid_argument{
                "Unexpected StateQt value: " + std::to_string(static_cast<int>(value))});
    }
}

} // namespace mpk::stable_link
