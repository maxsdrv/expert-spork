#pragma once

#include "exception/StringParseFailed.h"

#include <QMetaEnum>

namespace qt
{

template <typename Type>
QString metaEnumToString(Type type)
{
    return QMetaEnum::fromType<Type>().valueToKey(static_cast<int>(type));
}

template <typename Type>
Type metaEnumFromString(const QString& string)
{
    bool ok{false};
    auto value = QMetaEnum::fromType<Type>().keyToValue(string.toLatin1().data(), &ok);

    if (!ok)
    {
        BOOST_THROW_EXCEPTION(exception::StringParseFailed(string));
    }

    return static_cast<Type>(value);
}

template <typename E, typename F>
void iterateEnum(F func)
{
    auto metaEnum = QMetaEnum::fromType<E>();
    for (int i = 0; i < metaEnum.keyCount(); ++i)
    {
        func(metaEnum.key(i), static_cast<E>(metaEnum.value(i)));
    }
}

} // namespace qt
