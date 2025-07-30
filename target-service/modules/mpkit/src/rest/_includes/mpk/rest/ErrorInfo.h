#pragma once

#include <QString>

namespace mpk::rest
{

struct ErrorInfo
{
    QString message;
    int code;
};

} // namespace mpk::rest
