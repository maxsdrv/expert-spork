#pragma once

#include "HttpMethod.h"
#include "Permissions.h"

#include <QString>

namespace http
{

struct HttpRoute
{
    HttpMethod::Value method;
    QString path;
    Permissions permissions;
};

inline QString resolvePath(int version, const QString& path)
{
    return QString("/api/v%1/%2").arg(version).arg(path);
}

} // namespace http
