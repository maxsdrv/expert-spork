#pragma once

#include "json/composers/General.h"

template <>
struct ToJsonConverter<QByteArray>
{
    static QJsonValue convert(QByteArray const& array)
    {
        return json::toValue(QString(array.toBase64()));
    }
};
