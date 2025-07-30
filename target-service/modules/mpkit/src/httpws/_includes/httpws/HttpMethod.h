#pragma once

#include <QMetaType>
#include <QString>

namespace http
{

struct HttpMethod
{
    enum Value : uint8_t
    {
        PUT,
        GET,
        POST,
        DELETE
    };

    static QString toString(Value value);
    static Value fromString(const QString& string);
};

} // namespace http

Q_DECLARE_METATYPE(http::HttpMethod::Value) // NOLINT
