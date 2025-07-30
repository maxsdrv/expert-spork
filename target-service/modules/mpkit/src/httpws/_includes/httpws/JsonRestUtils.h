#pragma once

#include "json/JsonComposer.h"
#include "json/JsonParser.h"

namespace json
{

template <typename Value>
QJsonObject toDataObject(Value&& value)
{
    QJsonObject object;
    object.insert(QStringLiteral("data"), toValue(std::forward<Value>(value)));
    return object;
}

template <typename Value>
QJsonObject toDataObject(const QString& type, Value&& value)
{
    QJsonObject object;
    object.insert(QStringLiteral("type"), toValue(type));
    object.insert(QStringLiteral("data"), toValue(std::forward<Value>(value)));
    return object;
}

template <typename Value>
inline QJsonObject toDataObject(const char* type, Value&& value)
{
    return toDataObject(QString(type), std::forward<Value>(value));
}

template <typename Value>
inline Value fromDataObject(const QJsonObject& object)
{
    return fromObject<Value>(object, QStringLiteral("data"));
}

template <typename Value>
inline Value fromDataObject(const Value& /*value*/, const QJsonObject& object)
{
    return fromDataObject<Value>(object);
}

template <typename Value>
QJsonObject toValueObject(Value&& value)
{
    QJsonObject object;
    object.insert(QStringLiteral("value"), toValue(std::forward<Value>(value)));
    return object;
}

template <typename Value>
inline Value fromValueObject(const QJsonObject& object)
{
    return fromObject<Value>(object, QStringLiteral("value"));
}

QJsonObject toTypeObject(const QString& type);

} // namespace json
